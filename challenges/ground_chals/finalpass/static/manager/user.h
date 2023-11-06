#include <string>
#include <vector>

enum class EntryTypes:std::uint8_t{
    PASSWORD=0,
    FILE=1,
    NOTE=2,
    LAST=3
};

class Entry{
    public:
        Entry(EntryTypes entrytype, std::string& name, std::string& data);
        const std::string& GetData();
        const std::string& GetName();
    private:
        std::string data;
        std::string name;
        EntryTypes type;
};

class User{
    public:
        User(std::string &username, std::string &password_hash);
        // Change this to a string*** type?
        bool GetPasswordsEntryNames(std::vector<std::reference_wrapper<const std::string>>** entry_names);
        bool AddPasswordEntry(std::string &entry_name, std::string &password);
        bool DeletePasswordEntryByIndex(size_t idx);
        std::string* GetPasswordHash();
        const std::string* GetPasswordByIndex(size_t idx);
        std::string* GetUsername();
    private:
        void updateValues(std::string &username);
        std::string username;
        std::string password_hash;
        std::vector<Entry*> entries;
};