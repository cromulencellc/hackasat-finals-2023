#include "manager.h"

#include <assert.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "crypto.h"
#include "helper.h"

namespace fs = std::filesystem;

#define DB_BASE "./db/"

void menu(Manager *manager) {
    if (!manager) {
        LOG_ERR("Manager object not instantiated. Exiting..." CONTACT_ADMIN);
        exit(-1);
    }

    std::cout << std::endl;
    if (manager->GetCurrentUser()) {
        std::cout << "1: View password" << std::endl;
        std::cout << "2: Add password" << std::endl;
        std::cout << "3: Delete password" << std::endl;
        std::cout << "4: Delete current DB" << std::endl;
        std::cout << "5: Log out" << std::endl;
        std::cout << "> " << std::flush;
    } else {
        std::cout << "1: Login" << std::endl;
        std::cout << "2: Add new password database" << std::endl;
        std::cout << "3: List users" << std::endl;
        std::cout << "> " << std::flush;
    }
}

Manager::Manager(int port){
    this->team_port = port + 1;
}

User *Manager::ParseUserDB(std::string &buffer) {
    size_t tmp_len = 0;
    size_t length = buffer.size();

    if (length < 2 || (((tmp_len = *reinterpret_cast<const uint16_t *>(buffer.c_str())) != length))) {
        LOG_ERR("length(%lu) < 2 or tmp_len(%lu) != length (%lu)\n", length, tmp_len, length);
        return nullptr;
    }

    DBG("cur_len: 2 | %ld\n", length);
    if (sizeof(uint16_t) >= length) {
        return nullptr;
    }

    std::string::iterator it = std::find(buffer.begin() + sizeof(uint16_t), buffer.end(), '\0');
    if (it == buffer.end() || it == (buffer.begin() + 2)) {
        LOG_ERR("No username in buffer\n");

        return nullptr;
    }

    std::string username{buffer.begin() + 2, it};

    if (((it - buffer.begin()) + SHA256_DIGEST_LENGTH) >= length) {
        LOG_ERR("username_len + SHA256 len too long %ld | %ld\n", ((it - buffer.begin()) + SHA256_DIGEST_LENGTH), length);
        return nullptr;
    }

    it++;

    // Check to make sure the username is only alphanum
    if (std::find_if_not(username.begin(), username.end(), ::isalnum) != username.end()) {
        LOG_ERR("Non-alphanum character in provided username db\n");
        return nullptr;
    }

    std::string pass_hash = buffer.substr(it - buffer.begin(), SHA256_DIGEST_LENGTH);

    DBG("password hash len: %lu\n", pass_hash.size());

    it += SHA256_DIGEST_LENGTH;

    User *new_user = new User{username, pass_hash};

    // TODO: Fix this so its not constant size strings
    std::string::iterator null_it{0};

    size_t count = 0;
    while ((it - buffer.begin()) < length) {
        null_it = std::find(it, buffer.end(), '\0');
        if (null_it == buffer.end()) {
            LOG_ERR("Could not find name in db at entry: %lu\n", count);
            delete new_user;
            return nullptr;
        }
        std::string entry_name{it, null_it};

        it = null_it + 1;

        null_it = std::find(it, buffer.end(), '\0');
        if (null_it == buffer.end()) {
            LOG_ERR("Could not find password in db at entry: %lu\n", count);
            delete new_user;
            return nullptr;
        }

        std::string password{it, null_it};
        it = null_it + 1;

        new_user->AddPasswordEntry(entry_name, password);
        count++;
    }

    DBG("Parsed user DB. Length: %ld\n", length);

    return new_user;
}

size_t Manager::OpenDataConnection(uint8_t **data) {
    LOG("Opening data connection\n");
    // Open up another connection for transfering the DB

    int dataSock = socket(AF_INET, SOCK_STREAM, 0);
    if (dataSock < 0) {
        LOG_ERR("Could not open up socket." CONTACT_ADMIN);
        return 0;
    }

    int reuse = 1;
    if (setsockopt(dataSock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LOG("errno: %d\n", errno);
        LOG_ERR("Could not set socket options." CONTACT_ADMIN);
        return 0;
    }

    sockaddr_in serverAddr = {0}, clientAddr = {0};
    serverAddr.sin_family = AF_INET;
    // TODO: CHANGE TO htons(0);
    serverAddr.sin_port = htons(this->team_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(dataSock, (sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        LOG_ERR("Failed to bind socket\n" CONTACT_ADMIN);
        return 0;
    }

    if (listen(dataSock, 1) < 0) {
        LOG_ERR("Failed to listen on socket\n" CONTACT_ADMIN);
        return 0;
    }

    socklen_t clientLen = sizeof(clientAddr);
    if (getsockname(dataSock, (sockaddr *)&clientAddr, &clientLen) < 0) {
        LOG_ERR("Failed getsockname." CONTACT_ADMIN);
        return 0;
    }

    LOG("Listening for data on port: %d\n", ntohs(clientAddr.sin_port));

    int clientSock = accept(dataSock, (sockaddr *)&clientAddr, &clientLen);
    if (clientSock < 0) {
        LOG_ERR("Could not make connection to client." CONTACT_ADMIN);
        return 0;
    }

    uint64_t dataFileSize = 0, dataLen = 0;
    if (recv(clientSock, &dataFileSize, 4, 0) != 4) {
        close(clientSock);
        close(dataSock);
        LOG_ERR("Did not recieve proper size from client. Connection closed\n");
        return 0;
    }

    // TODO: Restrict max dataFileSize properly

    if (dataFileSize >= MAX_DATA_FILE_SIZE || dataFileSize == 0) {
        close(clientSock);
        close(dataSock);
        LOG_ERR("File size given too large. Connection closed\n");
        return 0;
    }

    *data = (std::uint8_t *)safe_calloc(1, dataFileSize);
    size_t len_data_received = 0;
    int i = 0;
    while (dataLen < dataFileSize) {
        if (((len_data_received = recv(clientSock, &(*data)[dataLen], dataFileSize - dataLen, 0)) < 0)) {
            LOG_ERR("Did not recieve all data for the file\n");
            free(*data);

            return 0;
        }

        dataLen += len_data_received;

        DBG("Recieved %ld bytes from socket. dataLen: %ld dataFileSize: %ld\n", len_data_received, dataLen, dataFileSize);
    }

    assert(dataLen == dataFileSize);

    LOG("Successfully received %ld bytes\n", dataLen);

    close(clientSock);
    close(dataSock);

    return dataLen;
}

// TODO: Is this needed??
std::vector<std::string> *Manager::GetListOfUsers(bool print_users = false) {
    std::string path(DB_BASE);
    std::string ext(".db");

    std::vector<std::string> *user_list = new std::vector<std::string>{};

    for (auto &p : fs::directory_iterator(path)) {
        if (p.path().extension() == ext) {
            DBG("user: %s\n", p.path().stem().string().c_str());
            if (print_users) {
                LOG(" %s\n", p.path().stem().string().c_str());
            }
            // TODO: Is this safe??
            user_list->push_back(p.path().stem().string());
        }
    }

    if (user_list->size() == 0) {
        delete user_list;
        return nullptr;
    }

    return user_list;
}

bool Manager::GetDBFromUser() {
    uint8_t *data = nullptr;

    size_t fileSize = this->OpenDataConnection(&data);
    if (fileSize == 0) {
        LOG_ERR("Unable to get DB from user.\n");
        return false;
    }

    std::string data_str{(char *)data, fileSize};
    User *newusrdb = this->ParseUserDB(data_str);
    if (!newusrdb) {
        LOG_ERR("Could not create user from DB.\n");
        return false;
    }

    this->current_username = *newusrdb->GetUsername();

    LOG("Possible new user: %s\n", this->current_username.c_str());
    
    // TODO: Is this needed??
    std::vector<std::string> *current_users = this->GetListOfUsers();
    if (current_users && std::find(current_users->begin(), current_users->end(), *newusrdb->GetUsername()) != current_users->end()) {
        LOG_ERR("User already exists!\n");
        delete current_users;
        delete newusrdb;
        return false;
    }

    // TODO: Make sure username is only alpha

    std::stringstream dbname;
    dbname << DB_BASE + *newusrdb->GetUsername() << ".db";

    LOG("Making USER DB %s\n\n", dbname.str().c_str());
    FILE *fd = fopen(dbname.str().c_str(), "w");

    if (fileSize != fwrite(data, 1, fileSize, fd)) {
        LOG_ERR("Writing to file failed. Bailing out..." CONTACT_ADMIN);
        exit(-1);
    }

    fclose(fd);

    delete newusrdb;

    return true;
}

void *Manager::StartDBThread(void *obj) {
    DBG("Making thread\n");
    std::cout.setstate(std::ios_base::failbit);

    Manager *manager = reinterpret_cast<Manager *>(obj);

    if (manager->GetDBFromUser()) {
        manager->current_user = manager->LoadCurrentUser();
    }

    std::cout.clear();
    menu(reinterpret_cast<Manager *>(obj));

    pthread_mutex_lock(&manager->lock);
    manager->thread_active = 0;
    pthread_mutex_unlock(&manager->lock);

    return nullptr;
}

// TODO: Do I need to handle if a thread is already active or not??
bool Manager::LoadDB() {
    pthread_t tid = {0};

    // TODO: Handle shared context obj
    // worker_thread_start

    pthread_mutex_lock(&this->lock);
    if (this->thread_active) {
        LOG_ERR("Thread already active.\n");
        pthread_mutex_unlock(&this->lock);
        return false;
    }

    this->thread_active = !pthread_create(&tid, NULL, StartDBThread, this);
    if (this->thread_active) {
        this->thread_id = tid;
    } else {
        LOG_ERR("Unable to spawn new thread." CONTACT_ADMIN);
        return false;
    }
    sleep(0.1);

    // Load the file as the DB
    pthread_mutex_unlock(&this->lock);

    return true;
}

User *Manager::LoadCurrentUser() {
    std::ifstream userdb;
    std::string dbname = DB_BASE + this->current_username;

    dbname.append(".db");

    LOG("Opening %s\n", dbname.c_str());

    userdb.open(dbname);

    if (!userdb) {
        std::cerr << "Could not open file: " << dbname << std::endl;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << userdb.rdbuf();

    userdb.close();

    std::string fin_buffer = buffer.str();

    User *cur_user = nullptr;
    if (!(cur_user = ParseUserDB(fin_buffer))) {
        return nullptr;
    }

    return cur_user;
}

// This can only be called when no user is logged in
bool Manager::Login() {
    std::string username;
    std::string password;

    flushcin();
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    DBG("username: %s | password: %s\n", username.c_str(), password.c_str());
    if (username.length() == 0 || password.length() == 0) {
        LOG_ERR("Username or password empty.\n");
        return false;
    }

    std::string::iterator it;
    if ((it = std::find_if_not(username.begin(), username.end(), ::isalnum)) != username.end()) {
        LOG_ERR("Non-alphanum character in username %d\n", *it);
        return false;
    }

    std::cout << "Attempting to log in " << username << std::endl;

    this->current_username = username;
    User *user = this->LoadCurrentUser();
    if (!user) {
        LOG_ERR("Could not login user as could not load database.\n");
        return false;
    }

    std::string *hash = Crypto::sha256(password);
    std::string *user_hash = user->GetPasswordHash();

    DBG("hash len: %d user_hash len: %d", hash->size(), user_hash->size());
    if (hash->size() != user_hash->size()) {
        LOG_ERR("Hash sizes do not match. " CONTACT_ADMIN);
        delete user;
        return false;
    }

    // Timing safe cmp
    int result = 0;
    for (std::size_t i = 0; i < hash->size(); i++) {
        result |= (*hash)[i] ^ (*user_hash)[i];
    }

    if (result) {
        LOG_ERR("Password did not match!\n");
        delete user;
        return false;
    }

    this->current_user = user;

    return true;
}

void Manager::LogOut() {
    delete this->current_user;

    this->current_user = nullptr;
}

bool Manager::ProcessChoice(std::uint8_t choice) {
    if (choice > static_cast<std::uint8_t>(Choices::LAST)) {
        choice = static_cast<std::uint8_t>(Choices::LAST);
    } else if (!this->GetCurrentUser()) {
        choice += static_cast<std::uint8_t>(Choices::END_LOGGED_IN);
    }

    Choices cur_choice{choice};

    bool ret = false;
    switch (cur_choice) {
        case Choices::VIEW_PASS:
            LOG("Viewing password entries\n");
            ret = this->ViewPassword();
            break;
        case Choices::ADD_PASS:
            LOG("Adding password\n");
            ret = this->AddPassword();
            break;
        case Choices::DELETE_PASS:
            LOG("Deleting password\n");
            ret = this->DeletePassword();
            break;
        case Choices::DELETE_DB:
            LOG("Deleting current DB\n");
            ret = this->DeleteDB();
            break;
        case Choices::LOG_OUT:
            LOG("Logging out\n");
            this->LogOut();
            ret = true;
            break;
        // Not Logged In
        case Choices::LOGIN:
            LOG("Logging in\n");
            ret = this->Login();
            break;
        case Choices::LOAD_DB:
            LOG("Adding DB\n");
            ret = this->LoadDB();
            break;
        case Choices::LIST_USERS:
            LOG("Listing users\n");
            ret = this->GetListOfUsers(true);
            break;
        case Choices::LAST:
        case Choices::END_LOGGED_IN:
        default:
            LOG("Invalid choice\n");
            ret = false;
    }

    return ret;
}

User *Manager::GetCurrentUser() {
    return this->current_user;
}

bool Manager::ViewPassword() {
    User *usr = this->GetCurrentUser();

    if (!usr) {
        LOG_ERR("Cannot view passwords. Not logged in.\n");
        return false;
    }

    std::vector<std::reference_wrapper<const std::string>> *entry_names = nullptr;

    bool not_empty = usr->GetPasswordsEntryNames(&entry_names);
    if (!not_empty) {
        LOG_ERR("Password entry list empty...\n");
        return false;
    }

    size_t i = 0;
    for (auto &entry : *entry_names) {
        std::cout << i << ": " << entry.get() << std::endl;
        i++;
    }

    size_t entry_num;

    std::cout << std::endl
              << "Password entry to view (0-" << entry_names->size() - 1 << ")"
              << "> ";
    std::cin >> entry_num;

    const std::string *password_entry = nullptr;

    if (!checkInput()) {
        LOG_ERR("Bad input while getting entry number\n");
        goto fail;
    } else if (entry_num >= entry_names->size()) {
        LOG_ERR("Entry num out of range\n");
        goto fail;
    }

    password_entry = usr->GetPasswordByIndex(entry_num);

    if (!password_entry) {
        LOG_ERR("Failed retrieving password entry: %ld\n", entry_num);
        goto fail;
    }

    std::cout << std::endl
              << (*entry_names)[entry_num].get() << ": " << *password_entry << std::endl;

    delete entry_names;
    return true;

fail:
    delete entry_names;
    return false;
}

bool Manager::AddPassword() {
    User *usr = this->GetCurrentUser();
    if (!usr) {
        LOG_ERR("Cannot add password. Not logged in\n");
        return false;
    }

    std::string entry_name, password;

    flushcin();
    std::cout << "Password name: ";
    std::getline(std::cin, entry_name);
    std::cout << "Password for " << entry_name << ": ";
    std::getline(std::cin, password);

    if (entry_name.length() == 0 || password.length() == 0) {
        LOG_ERR("Name/Password empty.\n");
        password.clear();

        return false;
    }

    std::vector<std::reference_wrapper<const std::string>> *entry_names = nullptr;

    usr->GetPasswordsEntryNames(&entry_names);
    if (!entry_names) {
        return false;
    }

    for (std::string cur_entry_name : *entry_names) {
        if (cur_entry_name.compare(entry_name) == 0) {
            LOG_ERR("Cannot add entry name that already exists.\n");
            goto fail;
        }
    }

    // TODO: Make sure addpasswordentry clones the password and does not std::copy
    // since the password is always cleared.
    if (!usr->AddPasswordEntry(entry_name, password)) {
        LOG_ERR("Unable to add password.\n");
        goto fail;
    }

    std::cout << "Added password for " << entry_name << std::endl;
    password.clear();
    delete entry_names;
    return true;

fail:
    password.clear();
    delete entry_names;
    return false;
}

bool Manager::DeletePassword() {
    User *usr = this->GetCurrentUser();

    size_t entry = 0;

    std::vector<std::reference_wrapper<const std::string>> *entry_names = nullptr;

    bool not_empty = usr->GetPasswordsEntryNames(&entry_names);
    if (!not_empty) {
        LOG_ERR("Password entry list empty...\n");
        return false;
    }
    std::cout << std::endl
              << "Password entry to delete (0-" << entry_names->size() - 1 << ")"
              << "> ";
    std::cin >> entry;

    if (!checkInput()) {
        LOG_ERR("Bad input while getting entry number\n");
        goto fail;
    } else if (entry >= entry_names->size()) {
        LOG_ERR("Entry num out of range\n");
        goto fail;
    }

    if (!usr->DeletePasswordEntryByIndex(entry)) {
        LOG_ERR("Could not delete password\n");
        goto fail;
    }

    std::cout << "Successfully deleted password at index " << entry << std::endl;
    delete entry_names;
    return true;

fail:
    delete entry_names;
    return false;
}

bool Manager::DeleteDB() {
    User* usr = this->GetCurrentUser();
    std::string username = *usr->GetUsername();
    std::string dbname = DB_BASE + username;
    dbname.append(".db");

    if (this->current_username == "admin") {
        LOG_ERR("Can't delete the admin DB!\n");
        return false;
    }
    else {
        if (!std::remove(dbname.c_str())) {
            this->LogOut();
            return true;
        }
        return false;
    }
}