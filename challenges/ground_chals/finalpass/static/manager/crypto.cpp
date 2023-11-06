#include "crypto.h"

// TODO: Return nullptr if any calls fail
std::string* Crypto::sha256(const std::string str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    SHA256_CTX sha256;

    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    std::string *final_hash = new std::string{(char*)hash, sizeof(hash)};

    // return ss.str();
    return final_hash;
}