#pragma once
#include "AppBase.hpp"
#include <functional>

// This is some functiod kung-fu that allows us to make message handling callbacks
namespace cromulence
{
    /// @brief This class creates a message handling object
    /// @details This is achieved by binding a message type, an object and a member function together and creating a functoid we can use in a table
    class Handler
    {
    public: 
        virtual void exe( CFE_SB_Msg_t *msgPtr )=0;
    };
    template<typename M , typename T , typename F > class MessageHandler : public Handler
    {
        public:
        MessageHandler( T* obj , F fcn )
        {   
            o = obj;
            f = fcn;
        }
        virtual void exe(  CFE_SB_Msg_t *msgPtr )
        {
            M msg( msgPtr );
            std::invoke( f , o , msg  );
            
        }
        T* o;
        F f;
    };
    /// @brief  The default message handler - logs an event
    class DefaultHandler : public Handler
    {
        public:
            virtual void exe( CFE_SB_Msg_t *msgPtr)
            {
                // uh-oh its an unknown return code!
                auto id = CFE_SB_GetMsgId( msgPtr );
                CFE_EVS_SendEvent(1, CFE_EVS_CRITICAL, "Unexpected message with id 0x%x",id);
                CFE_ES_WriteToSysLog("Unpected Message with id 0x%x", id );
            };
    };
    template<typename T, typename F> class BasicHandler : public Handler
    {
        public:
            BasicHandler( T* obj , F fcn )
            {   
                o = obj;
                f = fcn;
            }
            virtual void exe( CFE_SB_Msg_t *msgPtr)
            {

                std::invoke( f , o , msgPtr  );

            };
            T* o;
            F f;
    };
}