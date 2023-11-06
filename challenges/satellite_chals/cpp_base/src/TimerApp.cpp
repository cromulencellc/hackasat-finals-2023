#include "TimerApp.hpp"

namespace cromulence
{
    TimerApp::TimerApp( std::string appName , int32_t eventId ) :
        eventId_( eventId ),
        appName_( appName )
    {

    }

    TimerApp::~TimerApp()
    {

    }


    void TimerApp::execute( )
    {
        // lets make the function pointe rwith a lambda
        // this will remain as long as this function is in scope - which is the lifetime of the app
        auto functionPointer = [this]() {
                                this->onTimer(); };

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

            CFE_TIME_RegisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)&functionPointer);

            CFE_EVS_SendEvent(eventId_, CFE_EVS_INFORMATION, "Init...complete");
            runStatus_ = CFE_ES_RunStatus_APP_RUN;
            run = true;
        }

        
        run = CFE_ES_RunLoop(&runStatus_);
        while( true == run )
        {
            run = CFE_ES_RunLoop(&runStatus_);
        }
        // Remove the callback 
        CFE_TIME_UnregisterSynchCallback( (CFE_TIME_SynchCallbackPtr_t)&functionPointer );
        // Do any cleanup that occurs because of shutdown but not destruction
        this->shutdown();

        // Tell CFE we are good to be killed
        CFE_ES_ExitApp(runStatus_);           
    }
}