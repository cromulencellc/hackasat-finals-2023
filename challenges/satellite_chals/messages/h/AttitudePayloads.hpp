#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::attitude
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t EVT_ID = 64;
    static const size_t DEFAULT_BLIND = 600;
    // data structure
    struct OS_PACK Config
    {
        double max_blind;
        uint16_t frame_count;
        uint8_t enabled;
    };
    struct OS_PACK Attitude
    {
        double qAxis[3];
        double qScalar;
    };
    struct OS_PACK HouseKeeping
    {
        uint32_t accepted;
        uint32_t rejected;
        uint32_t blind_sec;
    };
}