#pragma once
#include "AppBase.hpp"

#include <string>
#include <functional>
#include <array> 
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <cxxabi.h>
#include <memory>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include "Handlers.hpp"
#include <chrono>


namespace cromulence
{


    /// /brief A CFS application for apps that talk via the messaging system
    /// /details This app will wake up whenver it receives a message or if it doesnt receive a messasge
    ///          within its timeout period. Whichever comes first.
    class ASyncApp : public cromulence::AppBase
    {
        public:

            /// /brief Constructor
            /// /param[in] depth The depth of the message queue
            /// /param[in] name The name of the app to be used by cfs
            /// /param[in] timeout The amount of time to wait for a message before you time out and check the app status
            /// /param[in] eventId (optional) All apps have an event ID associated with them - this defaults to 1 but you can use it to distinguish your apps by event id
            ASyncApp(uint16_t depth , std::string name , std::chrono::milliseconds timeout , uint16_t eventId=1 ) ;
            /// /brief Destructor
            virtual ~ASyncApp();

            /// @brief The apps main function
            /// @details This function contains the main run loop of the app therefore it will run until the app exits
            void execute();
            /// @brief  Allows the app to subscribe to message
            /// @param id The message id to which youd like to subscribe
            void subscribe( CFE_SB_MsgId_t id );
            /// @brief  Allows the app to subscribe to message
            /// @param id The message id to which youd like to subscribe
            /// @param depth The pipe depth for this specific message id
            void subscribe_ex( CFE_SB_MsgId_t id, uint16_t depth);
            /// @brief Allows the app to remove a message subscription
            /// @param id The message id to remove
            void unsubscribe( CFE_SB_MsgId_t id );
            /// @brief A Pure virtual function to initialzie the app
            /// @details This function gets called after construction but BEFORE the run loop executes
            virtual void initialize() =0 ;
            /// @brief A pure virtual function to shut down the app
            /// @details This function gets called after the run loop stops executing
            virtual void shutdown() = 0;
            /// @brief A virtual function that gets called if the timeout occurs without messages being recevied
            virtual void processTimeout( ){ }// leave this empty

        private:

        // i mean it - dont mess with these.
            CFE_SB_PipeId_t pipe_; ///< The CFS pipe we use
            uint16_t pipeDepth_; ///< How big is the CFS pipe
            std::string pipeName_;
            bool pipeCreated_; 
            uint32_t runStatus_;
            int32 timeout_;
            uint16_t eventId_;
        protected:
            
            std::unordered_map< CFE_SB_MsgId_t, Handler*> tlmHandlers_; ///< Command handlers for TLM messages
            std::unordered_map< CFE_SB_MsgId_t, Handler*> cmdHandlers_; ///< Command handlers for CMD messages
            DefaultHandler def_; ///< A default handler for messages that havent been registered
            std::shared_ptr< Handler > defaultHandler_;
             CFE_SB_MsgId_t cmdId_ = 0; 
            template<typename T,typename F> void default_message_handler( T *obj , F fcn )
            {
                defaultHandler_ = std::make_shared< BasicHandler<T,F> >( obj , fcn );
                
            }
            /// @brief Add a message handling function to the app
            /// @tparam M The message type
            /// @tparam T The object type
            /// @tparam F The member function type
            /// @param obj The object which owns the member function
            /// @param fcn The member function
            /// @param type Command or TLM
            template<typename M, typename T, typename F> void add_cmd_handler(T* obj,  F fcn)
            {
                using std::placeholders::_1;
                // Subscribe to the message
                this->subscribe_ex( M::MESSAGE_ID, pipeDepth_);
                // Create a message handling object
                auto h = new MessageHandler< M , T, F >( obj , fcn );
 
                // cOmmand messages have a command code and a function code
                cmdId_ = M::MESSAGE_ID;
                auto fcnId = M::FUNCTION_ID;
                cmdHandlers_.insert( { fcnId, h } );
                
            }
            template<typename M, typename T, typename F> void add_tlm_handler(T* obj,  F fcn)
            {
                using std::placeholders::_1;
                // Subscribe to the message
                this->subscribe_ex( M::MESSAGE_ID, pipeDepth_);

                // Create a message handling object
                auto h = new MessageHandler< M , T, F >( obj , fcn );
                // inert the handler depending on the type

                auto msgId = M::MESSAGE_ID;
                tlmHandlers_.insert( {msgId , h });
            }
            /// @brief The main message processing function
            /// @details This function is definied but is virtual in case you want an app with highly customized behavior
            ///          Not reccomended to overload this class.
            /// @param id The message ID
            /// @param fcnId The function code
            /// @param msg A pointer to the message
            virtual void processMessage( CFE_SB_MsgId_t id, uint16_t fcnId,   CFE_SB_Msg_t *msg );
            /// @brief Create sb receive pipe
            void create_sb_pipe();
    };


}