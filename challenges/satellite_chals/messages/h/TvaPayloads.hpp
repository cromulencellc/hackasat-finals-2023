#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::tva
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t FILE_PATH_MAX_SIZE = 32;
    static const size_t SYNC_WORD = 431942429;
    // data structure
    struct OS_PACK StartList
    {
        uint32_t num_cmds;
    };
    struct OS_PACK TimeMark
    {
        uint8_t yr;
        uint8_t mo;
        uint8_t day;
        uint8_t hr;
        uint8_t min;
        uint8_t sec;
    };
    struct OS_PACK Status
    {
        uint32_t num_loaded;
        uint32_t current_position;
        uint8_t load_status;
        char filename[FILE_PATH_MAX_SIZE];
    };
    struct OS_PACK File
    {
        char name[FILE_PATH_MAX_SIZE];
    };
    struct OS_PACK CommandMeta
    {
        double utc_sec;
        uint16_t command_id;
        uint8_t function;
        int64_t tGoMilliSec;
    };
}