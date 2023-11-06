#pragma once
#include <string>
#include <stdexcept>
#include <cstring>
#include "circular.hpp"
#include <memory>
extern "C" {
   #include "cfe.h"
}
namespace cromulence
{

        /// @brief  Takes async stream of bytes and turns it into command packets
        /// @tparam  
        /// @details No filtering or validation is applied. This class only turns raw bytes into packets
        template< size_t BUFFER_LEN , size_t PACKET_MAX_LEN  > class Packetizer
        {
            typedef cromulence::buffer::circular< uint8_t , BUFFER_LEN> ByteBuffa;
        public:
            Packetizer( uint32_t syncword  ) : 
                headerRdy_(false),
                packetRdy_(false),
                syncFound_(false),
                syncWord_( syncword)
            {
                buffer_ = std::make_shared< ByteBuffa >( );
            }
            ~Packetizer()
            {
                
            }

            void push_bytes( const void *ptr , size_t bytes )
            {
                const uint8_t *bytePtr = reinterpret_cast<const uint8_t*>( ptr );
                buffer_->push_back( bytePtr , bytes );
            }
            /// @brief Get the packet at the front of the line
            CFE_SB_Msg_t* get_packet()
            {
                CFE_SB_Msg_t* out;
                packetize( );

                if( ( true == headerRdy_ ) &&
                    ( true == packetRdy_ ) ) 
                {
                    out = reinterpret_cast<CFE_SB_Msg_t*>( packet_ ) ;
                }
                else
                {
                    out = nullptr;
                }
                return out;
                
            }
            void clear()
            {
                buffer_->clear();
                headerRdy_ = false;
                packetRdy_ = false;
                syncFound_ = false;
            }
            /// @brief  Pops the packet that is at the front of the line
            void pop_packet( )
            {
                      
                // reset the packet members
                std::memset( packet_ , 0 , PACKET_MAX_LEN );
                syncFound_ = false;
                headerRdy_ = false;
                packetRdy_ = false;
                
            }
            void print( )
            {
                buffer_->print();
            }
        protected: 


            void packetize( )
            {
                bool retry(true);
                while( true == retry )
                {
                    retry = false;
                    // we already completed a packet -- do nothing
                    if( true == packetRdy_ )
                    {
                        return; 
                    }
                    if( false == syncFound_ )
                    {
                        locateSync();
                    }
                    if( (false == headerRdy_) && (true == syncFound_) )
                    {
                        retry = unpackHeader( );
                    }
                    // if the header was completed then go ahead and try the res
                    if (true == headerRdy_ )
                    {
                        retry = unpackMessage();
                    }
                }
            }
            void locateSync( )
            {
                size_t syncLoc;
                uint8_t *s = reinterpret_cast< uint8_t*>( &syncWord_ );
                syncFound_ = buffer_->find_pattern( s , sizeof( syncWord_ ) ,syncLoc);
                if( true == syncFound_ )
                {
                    buffer_->remove( syncLoc );
                    buffer_->remove( sizeof(syncWord_) );    
                }
                else
                {
                    // cleanup the buffer
                    if( syncLoc > 0 )
                    {
                        buffer_->remove( syncLoc );
                    }
                }
            }
            bool unpackHeader( )
            {
                bool retry(false);
                // If we have enough bytes fill the header then do it
                if(  sizeof(CFE_SB_Msg_t ) <= buffer_->size() )
                {
                    // Check if there is another sync word within the header
                    bool found(false);
                    uint8_t *s = reinterpret_cast< uint8_t*>( &syncWord_ );
                    size_t syncLoc(0);
                    found = buffer_->find_pattern( s , sizeof( syncWord_ ) ,syncLoc);
                    if( (true == found) && ( syncLoc < sizeof(CFE_SB_Msg_t)))
                    {
                        // the header contains a sync word. We've got some messed up data
                        retry = true;
                        syncFound_ = false;
                    }
                    else
                    {
                        buffer_->pop_front( &packet_[0],  sizeof(CFE_SB_Msg_t) );
                        headerRdy_ = true;
                    }
                }
                return retry;
            }
            bool unpackMessage( )
            {
                bool retry(false);
                CFE_SB_Msg_t* msg;
                msg = reinterpret_cast< CFE_SB_Msg_t* >(  &packet_[0] );
                size_t len;
                uint16_t msgLen;
                msgLen = CFE_SB_GetTotalMsgLength(msg);
                len = msgLen - sizeof(CFE_SB_Msg_t);
                // lets see if a second sync word exists before the data length ends
                // if so that indicates a cutoff packet
                bool secondSync(false);
                uint8_t *s = reinterpret_cast< uint8_t*>( &syncWord_ );
                size_t syncLoc(0);
                secondSync = buffer_->find_pattern( s , sizeof( syncWord_ ) ,syncLoc);

                if( (true == secondSync) && syncLoc < len )
                {
                    // We have an incomplete packet - retry
                    syncFound_ = false;
                    headerRdy_ = false;
                    retry = true;
                }
                else if( len <= buffer_->size() )
                {
                    // We've got a full packet - its ready
                    buffer_->pop_front( &packet_[sizeof( CFE_SB_Msg_t)], len);
                    packetRdy_ = true;
                }
                else
                {
                    // packet is incomplete - do nothing
                }
                return retry;
            }

            // Byte bufferr
            std::shared_ptr< ByteBuffa > buffer_;
            //cromulence::buffer::circular< uint8_t , BUFFER_LEN>* buffer_;
            uint8_t packet_[PACKET_MAX_LEN];

            
            // Packet we are constructing
            bool headerRdy_;
            bool packetRdy_;
            bool syncFound_;
            uint32_t syncWord_;

        };
}