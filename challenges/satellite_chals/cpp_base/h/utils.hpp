#pragma once
// Global Imports
#include <filesystem>
#include <string>

// Local Imports
#include "osapi.h"

namespace cromulence{
    inline std::string resolve_path( std::string virt_path ){
        char tmp[256] = {0};
        OS_TranslatePath( virt_path.c_str() , tmp );
        std::string out( tmp);
        return std::filesystem::absolute( out );
    }
}
