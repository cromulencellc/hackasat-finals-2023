#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::script_udp
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t MAX_PORT = 9000;
    static const size_t TEAM_SIZE = 16;
    static const size_t MESSAGE_SIZE = 64;
    static const size_t PATH_SIZE = 256;
    // data structure
    struct OS_PACK Configure
    {
        uint16_t server_port;
    };
    struct OS_PACK Team
    {
        char name[TEAM_SIZE];
    };
    struct OS_PACK HouseKeeping
    {
        uint32_t gps_count;
        uint32_t image_count;
        uint32_t stage_count;
    };
    struct OS_PACK TxtMessage
    {
        char team[TEAM_SIZE];
        char message[MESSAGE_SIZE];
    };
}