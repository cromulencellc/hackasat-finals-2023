#pragma once

#include "AppBase.hpp"
#include <string>
#include "Handlers.hpp"
namespace cromulence
{


    /// /brief A CFS application that is rate based
    /// /details This application wakes up at a rate which is defined at consruction and something.
    ///          Application will run until CFE shuts it down
    template<typename T> class ClockFrameApp : public cromulence::AppBase
    {
        
        public:
            static T* obj;
            static int32 wrapper( )
            {
                ClockFrameApp::obj->onFrameBoundary();
                return 0;
            }
            ClockFrameApp( std::string appName , int32_t eventId ) : 
                eventId_( eventId ),
                appName_( appName )
            {

            }
            ~ClockFrameApp()
            {
                
            }

            // do this when you wake up
            virtual int32_t onFrameBoundary() = 0;
            //
            void execute()
            {

                bool run( false );

            int32_t  init_status        = CFE_SEVERITY_ERROR;
            runStatus_ = CFE_ES_APP_ERROR;

            // Register this application with the CFE 
            init_status = CFE_ES_RegisterApp();
            // Register with the event service system
            CFE_EVS_Register(NULL,0,0);

            if( CFE_SUCCESS == init_status )
            {
                // call the pure virtual initializeation
                CFE_EVS_SendEvent(eventId_, CFE_EVS_INFORMATION, "Initializing....");
                this->initialize();
                callback_ = &ClockFrameApp::wrapper;
                obj= dynamic_cast<T*>(this);
                CFE_TIME_RegisterSynchCallback(callback_);

                CFE_EVS_SendEvent(eventId_, CFE_EVS_INFORMATION, "Init...complete");
                runStatus_ = CFE_ES_RunStatus_APP_RUN;
                run = true;
            }


            run = CFE_ES_RunLoop(&runStatus_);
            while( true == run )
            {
                // this is going to run infinitely and we dont want it to eat up 
                // our processor so just check once a sec
                 OS_TaskDelay( 1000  );
                run = CFE_ES_RunLoop(&runStatus_);
            }
            // Remove the callback 
            CFE_TIME_UnregisterSynchCallback( (CFE_TIME_SynchCallbackPtr_t)&callback_ );
            // Do any cleanup that occurs because of shutdown but not destruction
            this->shutdown();

            // Tell CFE we are good to be killed
            CFE_ES_ExitApp(runStatus_);           
        }
            virtual void initialize() = 0;
            virtual void shutdown() = 0;
            void stop();


        protected:

            CFE_TIME_SynchCallbackPtr_t callback_;
            
            uint32_t runStatus_;
            uint16_t eventId_;
            std::string appName_;


    };
template<typename T> T* ClockFrameApp<T>::obj = nullptr;

}