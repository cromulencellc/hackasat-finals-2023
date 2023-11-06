#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <openssl/sha.h>
#include "helper.h"

class Crypto{
    public:
        static std::string* sha256(const std::string str);
};
