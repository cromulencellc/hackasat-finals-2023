#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::heartbeat
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    // data structure
}