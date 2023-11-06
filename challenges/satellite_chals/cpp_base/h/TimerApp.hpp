#pragma once

#include "AppBase.hpp"
#include <string>
#include "Handlers.hpp"
namespace cromulence
{


    /// /brief A CFS application that is rate based
    /// /details This application wakes up at a rate which is defined at consruction and something.
    ///          Application will run until CFE shuts it down

    
    class TimerApp : public cromulence::AppBase
    {
        
        public:
            TimerApp( std::string appName , int32_t eventId );
            ~TimerApp();

            // do this when you wake up
            virtual void onTimer() = 0;
            //
            void execute();
            virtual void initialize() = 0;
            virtual void shutdown() = 0;
            void stop();


        private:
            CFE_TIME_SynchCallbackPtr_t callback_;
            
            uint32_t runStatus_;
            uint16_t eventId_;
            std::string appName_;


    };

}