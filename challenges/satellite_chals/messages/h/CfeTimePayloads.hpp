#pragma once
extern "C" {
   #include "cfe.h"
}
namespace gsfc::cfe_time
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    // data structure
    struct OS_PACK Time
    {
        uint32_t seconds;
        uint32_t microseconds;
    };
}