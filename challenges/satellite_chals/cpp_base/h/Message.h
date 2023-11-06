#pragma once
extern "C" {
   #include "cfe.h"
}


#include <string>
#include <memory.h>
namespace cromulence
{

class MessageException : public std::exception
{
    public:
    MessageException(const char* error)
    {
        error_ = (char*)error;
    }
    char * error_;
    char * what ()
    {
        return error_;
    }
};

/// @brief An abstract base class that allows us to define a comman interface for messages
class BaseMessage 
{
    public:
        virtual void send()=0;
        virtual size_t size()=0;
        virtual const CFE_SB_Msg_t* as_cfs_ptr() = 0;
        virtual CFE_SB_MsgId_t msgId() = 0;
};

/// @brief Telemetry messages allows us to just specify the data payload and message ID as template parameters
/// @tparam T Payload structure (Make sure you use OS_PACK when you define it)
/// @tparam MSG_ID The message ID
template<typename T , CFE_SB_MsgId_t MSG_ID > class TlmMessage  : public BaseMessage
{

    public:
        // Data and the header
        typedef struct
        {
           CFE_SB_TlmHdr_t  header;
           T payload;
        } OS_PACK MsgStruct_t;
        // Make a new empty message
        TlmMessage()
        {
            memset( &data , 0 , sizeof( MsgStruct_t ));
            // Make sure that the message ID is a valid TLM id
            static_assert( ( MSG_ID & 0x1000) == 0 , "TLM messages must NOT have 0x1000 bit set");
            static_assert( (MSG_ID & 0x0800) != 0 , "TLM Messages must have bit 0x0800 set");
            // Initialize the message
            CFE_SB_InitMsg(&data, MSG_ID, THIS_MESSAGE_SIZE , TRUE);
            
        }

        /// @brief Create a TlmMsg from a raw CFE_SB_MSG pointer -copies the contents
        /// @param ptr Raw message pointer - copy its contents
        TlmMessage(const CFE_SB_Msg_t *ptr )
        {
            static_assert( ( MSG_ID & 0x1000) == 0 , "TLM messages must NOT have 0x1000 bit set");
            static_assert( (MSG_ID & 0x0800) != 0 , "TLM Messages must have bit 0x0800 set");

            const MsgStruct_t *in;
            in = reinterpret_cast<const MsgStruct_t * >( ptr );
            const CFE_SB_MsgId_t id = CFE_SB_GetMsgId( ptr );
            if( MESSAGE_ID != id )
            {
                throw MessageException("Message id mismatch") ;
            }
            data = *in;
        }
        ~TlmMessage()
        {

        }
        /// @brief Timestamp the message
        virtual void timestamp()
        {
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &data);
        }
        /// @brief Send the message
        virtual void send()
        {
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &data);
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &data);
        }
        /// @brief Get message size
        /// @return Total size of the message ( header and payload)
        virtual size_t size( ) 
        {
            return CFE_SB_GetTotalMsgLength( (CFE_SB_Msg_t *) &data );
        }
        /// @brief Get a raw cfs sb msg pointer to the messages data
        /// @return A pointer to the raw cfs message -gross
        virtual const CFE_SB_Msg_t* as_cfs_ptr()
        {
            return reinterpret_cast<const CFE_SB_Msg_t*>( &data);
        }
        virtual CFE_SB_MsgId_t msgId()
        {
            return MESSAGE_ID;
        }
        
        


        static const CFE_SB_MsgId_t MESSAGE_ID = MSG_ID;
        MsgStruct_t data;
    protected:

        static const uint32_t THIS_MESSAGE_SIZE = sizeof(MsgStruct_t); 

};

/// @brief Command messages allows us to just specify the data payload and message ID and function id as template parameters
/// @tparam T Data payload - make sure you use OS_PACK when you define this
/// @tparam MSG_ID The message ID
/// @tparam FCN_ID The function code
template<typename T , CFE_SB_MsgId_t MSG_ID , uint16_t FCN_ID = 0 > class CmdMessage : public BaseMessage
{


    public:
        typedef struct
        {
           CFE_SB_CmdHdr_t    header;
           T payload;
        } OS_PACK MsgStruct_t;
        // Make a new empty message
        CmdMessage()
        {
            memset( &data , 0 , sizeof( MsgStruct_t ));

            // Validate message ID at compile time
            static_assert( ( MSG_ID & 0x1000) != 0 , "Command messages must have 0x1000 bit set");
            static_assert( ( MSG_ID & 0x0800) != 0 , "Command messages must have 0x8000 bit set so the secondary header will be included");
            CFE_SB_InitMsg(&data, MSG_ID , THIS_MESSAGE_SIZE , TRUE);
            CFE_SB_SetCmdCode( (CFE_SB_Msg_t *)&data , FUNCTION_ID);
        }

        /// @brief Create a new command message and copy the contents from a raw CFS message pointer
        /// @param ptr Copy this messages contents
        CmdMessage(const CFE_SB_Msg_t *ptr )
        {
            static_assert( ( MSG_ID & 0x1000) != 0 , "Command messages must have 0x1000 bit set");
            static_assert( ( MSG_ID & 0x0800) != 0 , "Command messages must have 0x8000 bit set so the secondary header will be included");
            const MsgStruct_t *in;
            in = reinterpret_cast<const MsgStruct_t * >( ptr );
            const CFE_SB_MsgId_t id = CFE_SB_GetMsgId( ptr );
            if( MESSAGE_ID != id )
            {
                throw MessageException("Message id mismatch") ;
            }
            data = *in;
        }
        ~CmdMessage()
        {

        }
        /// @brief Calculate the messages checksum and insert into header
        void add_checksum()
        {
            CFE_SB_GenerateChecksum( (CFE_SB_Msg_t *) &data );

        }
        /// @brief Send the message to the CFS message bus
        /// @details Handles all timestamping and checksumming
        virtual void send()
        {
            CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &data);
            CFE_SB_GenerateChecksum( (CFE_SB_Msg_t *) &data );
            CFE_SB_SendMsg((CFE_SB_Msg_t *) &data);
        }
        /// @brief Get the size of the command message
        /// @return Total size headers and data
        virtual size_t size( )
        {
            return CFE_SB_GetTotalMsgLength( (CFE_SB_Msg_t *)&data );
        }
        /// @brief Get a pointer to the message data
        /// @return A raw CFS message pointer to the message header (gross)
        virtual const CFE_SB_Msg_t* as_cfs_ptr()
        {
            return reinterpret_cast<const CFE_SB_Msg_t*>( &data);
        }
        virtual CFE_SB_MsgId_t msgId()
        {
            return MESSAGE_ID;
        }
        static const CFE_SB_MsgId_t MESSAGE_ID = MSG_ID;
        static const uint16_t FUNCTION_ID = FCN_ID;
        MsgStruct_t data;
    protected:

        static const uint32_t THIS_MESSAGE_SIZE = sizeof(MsgStruct_t); 

};


}