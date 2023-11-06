#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::script
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t MAX_USER_SIZE = 16;
    static const size_t MAX_NAME_SIZE = 256;
    // data structure
    struct OS_PACK Script
    {
        uint8_t script;
    };
    struct OS_PACK User
    {
        char user[MAX_USER_SIZE];
    };
    struct OS_PACK CurrentScript
    {
        char script[MAX_NAME_SIZE];
        uint8_t alive;
        uint8_t status;
    };
}