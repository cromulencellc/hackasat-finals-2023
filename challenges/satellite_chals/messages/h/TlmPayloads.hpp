#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::tlm
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t MAX_BUFFERS = 10;
    // data structure
    struct OS_PACK FilterSettings
    {
        uint16_t apid;
        uint16_t depth;
        uint8_t decimation;
        uint8_t allowed;
        uint8_t queue;
    };
    struct OS_PACK FilterStatus
    {
        uint16_t apid;
        uint16_t depth;
        uint8_t decimation;
        uint8_t allowed;
        uint8_t queue;
        uint32_t decimationCtr;
    };
    struct OS_PACK FilterRequest
    {
        uint16_t apid;
    };
    struct OS_PACK BufferRequest
    {
        uint8_t bufferId;
    };
    struct OS_PACK BufferDetails
    {
        char path[32];
        char lastStore[32];
        uint8_t bufferId;
        uint32_t size;
    };
}