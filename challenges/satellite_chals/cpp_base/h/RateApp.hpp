#pragma once

#include "AppBase.hpp"

namespace cromulence
{


    /// /brief A CFS application that is rate based
    /// /details This application wakes up at a rate which is defined at consruction and something.
    ///          Application will run until CFE shuts it down
    class RateApp : public cromulence::AppBase
    {
        
        public:
            RateApp( uint32_t updatePeriodUs );
            ~RateApp();

            // do this when you wake up
            virtual void atRate() = 0;
            //
            void execute();
            virtual void initialize() = 0;
            virtual void shutdown() = 0;
            void stop();
        private:
            // I mean it - we dont want users mucking with this
            uint32_t localRunStatus_;
            uint32_t updatePeriodMs_;
    };

}