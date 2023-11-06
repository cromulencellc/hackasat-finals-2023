#include "RateApp.hpp"

namespace cromulence
{
       // A rate based application
    RateApp::RateApp( uint32_t updatePeriodMs ) : AppBase(),
        updatePeriodMs_(updatePeriodMs)
    {


    }

    RateApp::~RateApp()
    {

    }

    void RateApp::execute()
    {
        int32_t  init_status        = CFE_SEVERITY_ERROR;
        localRunStatus_ = CFE_ES_APP_ERROR;

        bool run(false);
        // Register this application with the CFE 
        init_status = CFE_ES_RegisterApp();
        // Register with event services
        CFE_EVS_Register(NULL,0,0);

        if( CFE_SUCCESS == init_status )
        {
            // call the pure virtual initializeation
            this->initialize();
            localRunStatus_ = CFE_ES_RunStatus_APP_RUN;
        }

        // we want to run the loop at least one time
        
        run = CFE_ES_RunLoop(&localRunStatus_);
        while( true == run )
        {
            // do whatever you are supposed to do
            this->atRate();
            // sleep for some amount of time
            OS_TaskDelay( updatePeriodMs_ );
            // check if we need to stop 
            run = CFE_ES_RunLoop(&localRunStatus_);
        }

        this->shutdown();
        /// Let cFE kill this task and any child tasks
        CFE_ES_ExitApp(localRunStatus_);             


    }   
    void RateApp::stop()
    {
        localRunStatus_ = CFE_ES_RunStatus_APP_EXIT;        
    }
}