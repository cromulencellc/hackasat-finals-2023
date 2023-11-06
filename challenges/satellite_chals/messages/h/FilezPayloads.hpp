#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::filez
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t PATH_SIZE = 64;
    static const size_t MAX_FILENAME_SIZE = 32;
    static const size_t MAX_FILE_LIST = 10;
    static const size_t SHA_SIZE = 32;
    // data structure
    struct OS_PACK DirRequest
    {
        char path[PATH_SIZE];
        char pattern[32];
    };
    struct OS_PACK Snippet
    {
        char filename[MAX_FILENAME_SIZE];
        uint32_t totalFileSize;
        uint32_t startSeek;
        uint32_t packetSize;
        uint8_t packet[1024];
        uint8_t packetStatus;
    };
    struct OS_PACK Delete
    {
        char path[PATH_SIZE];
    };
    struct OS_PACK Copy
    {
        char src[PATH_SIZE];
        char dst[PATH_SIZE];
        uint8_t overwrite;
    };
    struct OS_PACK Response
    {
        char path[PATH_SIZE];
        uint32_t totalFileSize;
        char currentSha[SHA_SIZE];
    };
    struct OS_PACK ResponseShort
    {
        char path[PATH_SIZE];
        uint32_t totalFileSize;
        uint32_t crc32;
    };
    struct OS_PACK File
    {
        char path[64];
        uint32_t size;
    };
    struct OS_PACK FileList
    {
        File ls[MAX_FILE_LIST];
    };
    struct OS_PACK UnPack
    {
        char path[PATH_SIZE];
        char file[MAX_FILENAME_SIZE];
    };
}