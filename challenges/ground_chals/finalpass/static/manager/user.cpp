#include "user.h"

#include <functional>

#include "helper.h"

Entry::Entry(EntryTypes entrytype, std::string& name, std::string& data) {
    if (entrytype >= EntryTypes::LAST) {
        // this->data = nullptr;
        // this->name = nullptr;
        this->type = EntryTypes::LAST;
        return;
    }
    this->type = entrytype;
    this->data = std::move(data);
    this->name = std::move(name);
}

const std::string& Entry::GetData() {
    return this->data;
}

const std::string& Entry::GetName() {
    return this->name;
}

User::User(std::string& username, std::string& password_hash) {
    this->username = std::move(username);
    this->password_hash = std::move(password_hash);

    return;
}

bool User::GetPasswordsEntryNames(std::vector<std::reference_wrapper<const std::string>>** entry_names) {
    *entry_names = new std::vector<std::reference_wrapper<const std::string>>{};

    if (this->entries.size() == 0) {
        return false;
    }

    for (Entry* entry : this->entries) {
        (*entry_names)->push_back(entry->GetName());
    }

    return true;
}

bool User::AddPasswordEntry(std::string& entry_name, std::string& password) {
    if (entry_name.length() == 0 || password.length() == 0) {
        return false;
    }

    Entry* pw_entry = new Entry{EntryTypes::PASSWORD, entry_name, password};
    this->entries.push_back(pw_entry);

    return true;
}

bool User::DeletePasswordEntryByIndex(size_t idx) {
    if (idx >= this->entries.size()) {
        LOG_ERR("Could not find password entry. Index %lu outside of range %lu.", idx, this->entries.size());

        return false;
    }

    this->entries.erase(std::next(this->entries.begin(), idx));

    return true;
}

std::string* User::GetPasswordHash() {
    return &this->password_hash;
}

const std::string* User::GetPasswordByIndex(size_t idx) {
    if (idx >= this->entries.size()) {
        return nullptr;
    }

    return &(this->entries[idx]->GetData());
}

std::string* User::GetUsername() {
    return &this->username;
}

void User::updateValues(std::string& username) {
    this->username = std::move(username);
}