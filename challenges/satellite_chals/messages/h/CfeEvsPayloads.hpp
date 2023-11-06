#pragma once
extern "C" {
   #include "cfe.h"
}
namespace gsfc::cfe_evs
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    // data structure
    struct OS_PACK Event
    {
        char app_name[20];
        uint16_t event_id;
        uint16_t event_type;
        uint32_t spacecraft_id;
        uint32_t processor_id;
        char message[976];
        uint16_t spare;
    };
}