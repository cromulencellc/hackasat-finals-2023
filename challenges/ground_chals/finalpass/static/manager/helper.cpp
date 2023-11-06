#include "helper.h"
#include <iomanip>
#include <string>
#include <sstream>

void flushcin(){
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool checkInput(bool exit_program){
    if( std::cin.fail() || std::cin.eof() )
    {
        flushcin();

        std::cerr << "bad input" << std::endl;
        if(exit_program){
            std::cerr << "Exiting..." << std::endl;
            exit(1);
        }
    }

    return true;
}

void* safe_malloc(size_t length){
    void *ptr = malloc(length);
    if(!ptr){
        LOG_ERR("Could not allocate space. Bailing out...\n");
        exit(-1);
    }

    return ptr;
}

void* safe_calloc(size_t nmemb, size_t length){
    void *ptr = calloc(nmemb, length);
    if(!ptr){
        LOG_ERR("Could not allocate space. Bailing out...\n");
        exit(-1);
    }

    return ptr;
}

std::string hexlify(std::string input){
    if(input.length() == 0)
        return nullptr;

    std::stringstream ss;
    for(int i = 0; i < input.length(); i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)input[i];
    }

    return ss.str();
}