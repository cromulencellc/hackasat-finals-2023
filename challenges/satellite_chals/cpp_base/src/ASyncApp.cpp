#include "ASyncApp.hpp"

#include <exception>
#include <stdexcept>
#include "str.hpp"
namespace cromulence
{
    ASyncApp::ASyncApp( uint16_t depth , std::string name ,std::chrono::milliseconds timeout , uint16_t eventId ) : cromulence::AppBase(),
        pipeDepth_( depth ),
        pipeName_( name ),
        pipeCreated_(false),
        eventId_( eventId )
        
    {
        timeout_ = timeout.count();
        // setup the default message handler
        defaultHandler_ = std::make_shared< DefaultHandler >();
    }
    ASyncApp::~ASyncApp()
    {
        // Pipes need to be expltally managed - maybe we should do this with RAII
        if( true == pipeCreated_)
        {
            CFE_SB_DeletePipe( pipe_ );
        }
    }
    void ASyncApp::create_sb_pipe() {
        // Check if we need to create the pipe
        if( false == pipeCreated_ )
        {
            // CFE_ES_WriteToSysLog("CFE_SB_CreatePipe. Depth %d, Name: %s\n", pipeDepth_, pipeName_.c_str());
            int status = CFE_SB_CreatePipe(&pipe_, pipeDepth_, pipeName_.c_str() );
            if (status == CFE_SUCCESS) {
                pipeCreated_ = true;

            } else {
                CFE_EVS_SendEvent(eventId_, CFE_EVS_ERROR, "Could not create pipe....");
            }

        }
    }
    void ASyncApp::subscribe( CFE_SB_MsgId_t id )
    {
        this->create_sb_pipe();
        // TODO: make sure we dont duplicate the subscription
        // subscribe to the message
        if(CFE_SB_Subscribe(id, pipe_ ) != CFE_SUCCESS){
            CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "SB Subscribe Error" );
        };
    }
    void ASyncApp::subscribe_ex( CFE_SB_MsgId_t id, uint16_t depth)
    {
        this->create_sb_pipe();
        // TODO: make sure we dont duplicate the subscription
        // subscribe to the message
        CFE_SB_Qos_t qos = {0, 0};
        // OS_printf("Subscribe Ex. Id %d, Depth %d\n", id, depth);
        if(CFE_SB_SubscribeEx(id, pipe_ , qos, depth) != CFE_SUCCESS) {
            CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "SB Subscribe Ex Error" );
        }
    }
    void ASyncApp::unsubscribe( CFE_SB_MsgId_t id )
    {
        // Just unsubscribe
        CFE_SB_Unsubscribe(id, pipe_ );
    }
    
    void ASyncApp::execute()
    {
        CFE_SB_Msg_t *msgPtr;

        int32_t  init_status        = CFE_SEVERITY_ERROR;
        runStatus_ = CFE_ES_APP_ERROR;

        bool run(false);
        // Register this application with the CFE 
        init_status = CFE_ES_RegisterApp();
        // Register with the event service system
        CFE_EVS_Register(NULL,0,0);

        if( CFE_SUCCESS == init_status )
        {
            // call the pure virtual initializeation
            CFE_EVS_SendEvent(eventId_, CFE_EVS_INFORMATION, "Initializing....");
            try
            {
                this->initialize();
                CFE_EVS_SendEvent(eventId_, CFE_EVS_INFORMATION, "Init...complete");
                runStatus_ = CFE_ES_RunStatus_APP_RUN;
                run = true;
            }
            catch( ... )
            {
                CFE_EVS_SendEvent(eventId_, CFE_EVS_INFORMATION, "Throw in init");
                run = false;
                runStatus_ = CFE_ES_RunStatus_APP_ERROR;
            }
            
        }

        // Execute the run loop until we are told to stop
        run = CFE_ES_RunLoop(&runStatus_);
        while( true == run )
        {
            int32_t status;

            // wait until we receive a message or hit the timeout
            status = CFE_SB_RcvMsg(&msgPtr, pipe_, timeout_);
            switch(status)
            {
                case CFE_SUCCESS:
                    CFE_SB_MsgId_t id; 
                    uint16_t fcnId;
                    id = CFE_SB_GetMsgId(msgPtr);
                    fcnId = CFE_SB_GetCmdCode(msgPtr);   
                    // If we got a message we should call process message
                    // This is in a try catch so that the children can throw safely
                    try
                    { 
                        this->processMessage(id, fcnId,  msgPtr );
                    }
                    catch (std::exception &e)
                    {
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "Async app threw while processing message id: %d fcn: %d", id , fcnId );
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "Exception: %s", e.what() );
                        
                    }
                    catch (...)
                    {
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "Async app threw while processing message id: %d fcn: %d", id , fcnId );
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "AsyncApp uknown throw" );
                        
                    }
                    break;
                case CFE_SB_BAD_ARGUMENT :
                    CFE_EVS_SendEvent(eventId_, CFE_EVS_CRITICAL, "AyncApp has an message with bad argument");

                    break;
                case CFE_SB_TIME_OUT:
                    // If we hit the timeout call the timeout processor
                    try
                    {
                        this->processTimeout();
                    }
                    catch (std::exception &e)
                    {
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "Async app threw while processing timeout");
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "Exception: %s", e.what() );
                        
                    }
                    catch (...)
                    {
                        CFE_EVS_SendEvent( eventId_ , CFE_EVS_CRITICAL , "AsyncApp uknown throw" );
                        
                    }                  
                    
                    break;
                case CFE_SB_PIPE_RD_ERR:
                    CFE_EVS_SendEvent(eventId_, CFE_EVS_CRITICAL, "AyncApp has an unexpected read error on the message pipe?!");
                    break;
                case CFE_SB_NO_MESSAGE:
                    if (timeout_ != CFE_SB_POLL) {
                        CFE_EVS_SendEvent(eventId_, CFE_EVS_CRITICAL, "AsyncApp no message returned even though we have a timeout?!");
                    }
                    break;
                default:
                    // uh-oh its an unknown return code!~
                    CFE_EVS_SendEvent(eventId_, CFE_EVS_CRITICAL, "Messaging system returned an unknown status! WTF!?");
                    CFE_ES_WriteToSysLog("Messaging system returned an unknown status! WTF!?");
                    break;
            }
            // Check the run status and deduce if we should shutdown
            run = CFE_ES_RunLoop(&runStatus_);

        }
        // Do any cleanup that occurs because of shutdown but not destruction
        this->shutdown();

        // Tell CFE we are good to be killed
        CFE_ES_ExitApp(runStatus_);             
        
    }
    void ASyncApp::processMessage( CFE_SB_MsgId_t id, uint16_t fcnId,   CFE_SB_Msg_t *msg )
    {
        // Our app is allowed a single command ID.
        // If the message id matches the command id then use the command handlers
        if( id == cmdId_ )
        {
            if( cmdHandlers_.contains( fcnId ) )
            {
                // CFE_ES_WriteToSysLog("Process command msgid 0x%04X, Fcn %d\n", id, fcnId);
                cmdHandlers_[fcnId]->exe(msg);
            }
            else
            {
                CFE_EVS_SendEvent(eventId_, CFE_EVS_CRITICAL, "Unhandled command rcvd 0x%x, 0x%x",id ,fcnId );
            }
        }
        else if( tlmHandlers_.contains( id ) )
        {
            // otherwise see if its a tlm message we registered for
            tlmHandlers_[id]->exe(msg);
        }
        else 
        {
            // this is a message we got that has no handler -- use the default
            defaultHandler_->exe( msg );
        }   
 
    }

}