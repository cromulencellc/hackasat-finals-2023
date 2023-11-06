#include <pthread.h>

#include <iostream>
#include <string>

#include "user.h"

#define MAX_DATA_FILE_SIZE 65535

enum class Choices : std::uint8_t {
    // LOGGED IN
    VIEW_PASS = 1,
    ADD_PASS = 2,
    DELETE_PASS = 3,
    DELETE_DB = 4,
    LOG_OUT = 5,
    END_LOGGED_IN = 6,
    // NOT LOGGED IN
    LOGIN = 7,
    LOAD_DB = 8,
    LIST_USERS = 9,
    LAST = 10
};

class Manager {
   public:
    Manager(int port);
    bool Login();
    void LogOut();

    bool AddPassword();
    bool DeletePassword();
    bool ViewPassword();
    bool DeleteDB();

    bool GetDBFromUser();
    bool LoadDB();

    User* LoadCurrentUser();
    User* GetCurrentUser();
    std::vector<std::string>* GetListOfUsers(bool print_users);

    bool ProcessChoice(std::uint8_t choice);
    size_t OpenDataConnection(uint8_t** data);
    static void* StartDBThread(void* obj);
    int team_port;
   private:
    User* ParseUserDB(std::string& buffer);
    User* current_user;
    std::string current_username;
    pthread_mutex_t lock;
    int thread_active;
    pthread_t thread_id;
};

void menu(Manager* manager);