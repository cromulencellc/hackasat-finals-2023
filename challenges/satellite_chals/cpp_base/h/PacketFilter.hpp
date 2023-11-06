#pragma once

extern "C" {
   #include "cfe.h"
}
#include <string>
#include <array>
namespace cromulence
{
    namespace packets
    {
        
        /// @brief An object for filtering packets based on their message id
        class Filter
        {
        public:
            struct FilterStatus
            {
                bool allowed; //< Should the message be allowed to continue
                int route; //< Where should we send the message (ie which tlm buffer or device)
                int decimation; //< How often should we decimate this data
                int decimationCtr; //< How many have we receoved
                int bufferDepth; //< Buffer Depth for message
            };
            /// @brief Create the filte object
            Filter( );

            ~Filter();
            /// @brief Initialize the filter object from a json file name
            /// @param filename The json file with the filter settings 
            void from_json( std::string filename );
            /// @brief Modify if a specific message id is allowed
            /// @param apid Modify this message id
            /// @param allowed Is it allowed
            void modify_filter( uint16_t apid , bool allowed );
            /// @brief Modify a specific message ids decimation rate
            /// @param apid Which messag id
            /// @param decimation new decimation rate
            void modify_decimation( uint16_t apid , uint8_t decimation);
            /// @brief Mopdify a specif message ids route
            /// @param apid Which message id
            /// @param route new route
            void modify_route( uint16_t apid , uint8_t route);
            /// @brief Mopdify a specif message ids subscription depth
            /// @param apid Which message id
            /// @param depth new route
            void modify_depth( uint16_t msgid , uint16_t depth);
            /// @brief Reset all counters
            void reset() ;
            /// @brief Return everything to default (not allowed)
            void defaults();

             
            bool valid_checksum(const CFE_SB_Msg_t* msg );
            static bool is_cmd(const  CFE_SB_Msg_t* msg );
            static bool is_tlm(const  CFE_SB_Msg_t* msg );
            static bool is_cmd(const CFE_SB_MsgId_t id );
            static bool is_tlm(const CFE_SB_MsgId_t id );
            bool allowed(const  CFE_SB_Msg_t* msg);
            bool allowed( const CFE_SB_MsgId_t id );
            bool ack_allowed( const CFE_SB_Msg_t* msg );

            bool decimated(const  CFE_SB_Msg_t* msg);
            int depth(const CFE_SB_MsgId_t id );
            int  routing( const CFE_SB_Msg_t* msg );
            uint16_t as_tlm_id( uint16_t apid );
            static uint16_t apid( const CFE_SB_MsgId_t id );
        
            FilterStatus status( uint16_t apid );
            static const uint16_t APID_MASK = 0x7FF;

            static const uint16_t MAX_APID = APID_MASK + 1;

        protected:
            static const uint16_t CMD_MASK = 0x1000;
            static const uint16_t SEC_HEADER_MASK = 0x0800;
            // Using a std::array here will provide very fast O(1) lookup speed and O(N) with memory
            // this will be very greedy with the memory - since MAX_APID is 2048 that is ok
            std::array< bool, MAX_APID > allowedList_;  
            std::array< bool , MAX_APID > ackAllowedList_;
            // decimation
            std::array< uint16_t , MAX_APID > decimationCounter_;
            std::array< uint16_t , MAX_APID > decimationRate_;
            std::array< uint16_t , MAX_APID > routingTable_;
            std::array< uint16_t , MAX_APID > depthTable_;
            std::array< uint16_t , MAX_APID > msgIdTable_;


        };
    }
}
