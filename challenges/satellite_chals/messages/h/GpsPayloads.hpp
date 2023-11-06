#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::gps
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t EVT_ID = 201;
    static const size_t DATUM_LEN = 16;
    static const size_t SOLN_LEN = 32;
    // data structure
    struct OS_PACK Config
    {
        uint8_t housekeeping_rate;
        uint8_t polling_rate;
        uint8_t polling_enabled;
    };
    struct OS_PACK TimeData
    {
        uint8_t yr;
        uint8_t mo;
        uint8_t day;
        uint8_t hr;
        uint8_t min;
        uint8_t sec;
        uint8_t csec;
    };
    struct OS_PACK Position
    {
        double position[3];
    };
    struct OS_PACK LLH
    {
        float longitude;
        float latitude;
        float altitude;
        float dev_lat;
        float dev_lon;
        float dev_alt;
        float soln_age;
        uint8_t sats_tracked;
        uint8_t sats_soln;
        char solution_info[SOLN_LEN];
        char datum[DATUM_LEN];
    };
}