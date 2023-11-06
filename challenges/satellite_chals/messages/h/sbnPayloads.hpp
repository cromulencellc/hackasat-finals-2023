#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::sbn
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t PATH_SIZE = 256;
    // data structure
}