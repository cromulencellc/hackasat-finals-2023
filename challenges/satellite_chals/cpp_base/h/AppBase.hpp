#pragma once

extern "C" {
   #include "cfe.h"
}

namespace cromulence
{
    
    class AppBase
    {
        public:

            AppBase();
            ~AppBase();

            virtual void execute() = 0;
            virtual void initialize() = 0;      
            virtual void shutdown() = 0;


    };

}