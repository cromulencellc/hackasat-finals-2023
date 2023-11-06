#pragma once
extern "C" {
   #include "cfe.h"
}
namespace cromulence::version
{
    typedef uint8_t empty[0];
    static_assert( sizeof(empty)==0, "EMPTYS MUST BE SIZE 0");
    static const size_t MAX_NAME_SIZE = 32;
    static const size_t MAX_APPS = 10;
    static const size_t SHA_SIZE = 32;
    static const size_t FPGA_NAME_SIZE = 40;
    static const size_t TIME_SIZE = 10;
    static const size_t DATE_SIZE = 16;
    // data structure
    struct OS_PACK VersionInfo
    {
        char sw_build_date[DATE_SIZE];
        char sw_build_time[TIME_SIZE];
        char fpga_version[FPGA_NAME_SIZE];
    };
    struct OS_PACK AppName
    {
        char app[MAX_NAME_SIZE];
    };
    struct OS_PACK Sha
    {
        char sha[SHA_SIZE];
    };
    struct OS_PACK AppInfo
    {
        AppName app_names[MAX_APPS];
        Sha app_sha[MAX_APPS];
    };
}