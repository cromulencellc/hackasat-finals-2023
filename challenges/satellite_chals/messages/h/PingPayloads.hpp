#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::ping
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t PING_SIZE = 256;
    // data structure
    struct OS_PACK SpamReq
    {
        uint16_t nToSpam;
    };
    struct OS_PACK Spam
    {
        uint8_t kb[1012];
    };
    struct OS_PACK SmolPing
    {
        char time[32];
    };
    struct OS_PACK Ping
    {
        char data[PING_SIZE];
    };
    struct OS_PACK Pong
    {
        char time[PING_SIZE];
        char data[256];
    };
    struct OS_PACK SmolPong
    {
        char time[32];
    };
    struct OS_PACK HouseKeeping
    {
        uint32_t pingsRcvd;
        uint32_t invalidMsgs;
        double timeLastPing;
    };
}