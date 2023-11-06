#pragma once 
#include <string>
#include <cstring>
namespace cromulence
{

    inline void strncpy_nt(  char* dest, const char* src,  size_t max_len )
    {
        // forces a utf-8 compliant terminated str cpy
        strncpy( dest, src , max_len-1 );
        dest[max_len-1] = '\0';
    }
}