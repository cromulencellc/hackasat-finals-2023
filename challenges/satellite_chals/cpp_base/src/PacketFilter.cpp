#include "PacketFilter.hpp"
#include <sstream>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace cromulence::packets
{
    Filter::Filter( )
    {
        allowedList_.fill( false );
        decimationRate_.fill( 1 );
        routingTable_.fill( 0 );
        ackAllowedList_.fill( true );
    }
    Filter::~Filter()
    {

    }
    void Filter::from_json( std::string configFile )
    {
        
        std::string err;
        std::ifstream f( configFile.c_str() );
        json data =  json::parse(f);
        
        allowedList_.fill( false );
        decimationRate_.fill( 1 );
        decimationCounter_.fill( 0 );
        routingTable_.fill( 0 );
        depthTable_.fill( 0 );
        
        auto packetList = data["packets"];
        for( auto& [key, value]  : packetList.items() )
        {
            uint16_t apid;
            
            int index = value["id"];
            apid = ( index & APID_MASK );
            allowedList_.at( apid )    = true;
            decimationRate_.at( apid ) = value["decimation"];
            routingTable_.at( apid )   = value["route"];
            depthTable_.at( apid )     = value["depth"];
            ackAllowedList_.at( apid ) = value["ack"];
        }
        
    }
    void Filter::modify_filter( uint16_t msgid , bool allowed )
    {
        uint16_t apid = msgid & APID_MASK;
        allowedList_.at( apid ) = allowed;
    }
    void Filter::modify_decimation( uint16_t msgid , uint8_t decimation)
    {
        uint16_t apid = msgid & APID_MASK;
        decimationRate_.at( apid ) = decimation;
        decimationCounter_.at( apid ) = 0;
    }
    void Filter::modify_route( uint16_t msgid , uint8_t route)
    {
        uint16_t apid = msgid & APID_MASK;
        routingTable_.at( apid ) = route;
    }
    void Filter::modify_depth( uint16_t msgid , uint16_t depth)
    {
        uint16_t apid = msgid & APID_MASK;
        depthTable_.at( apid ) = depth;
    }
    void Filter::reset() 
    {
        // reset our counters
        decimationCounter_.fill( 0 );

    }
    bool Filter::is_cmd(const  CFE_SB_Msg_t* msg )
    {
        uint16_t apid;
        apid = CFE_SB_GetMsgId( msg );
        return (  apid & CMD_MASK ) == CMD_MASK;
    }
    bool Filter::is_tlm(const  CFE_SB_Msg_t* msg )
    {
        uint16_t apid;
        apid = CFE_SB_GetMsgId( msg );
        return (  apid & CMD_MASK ) == 0;
    }
    bool Filter::is_cmd( const CFE_SB_MsgId_t id )
    {   
        return (  id & CMD_MASK ) == CMD_MASK;
    }
    bool Filter::is_tlm( const CFE_SB_MsgId_t id )
    {
        return ( id & CMD_MASK ) == 0;
    }
    uint16_t Filter::apid( const CFE_SB_MsgId_t id )
    {
        constexpr uint16_t mask = ~( CMD_MASK  | SEC_HEADER_MASK );
        return ( id & mask );
    }
    bool Filter::valid_checksum(const CFE_SB_Msg_t* msg )
    {
        bool ok(false);
        // Const cast is not good to use because it removes protections
        // but the very cool cfs doesnt use cosnt pointers where it should
        // so T_T
        CFE_SB_Msg_t *m = const_cast<CFE_SB_Msg_t *>(msg);
        ok = CFE_SB_ValidateChecksum( m );
        if (false == ok) {
            CFE_EVS_SendEvent( 1 , CFE_EVS_ERROR , "Recieved Command with Invalid Checksum. MsgId 0x%04X", CFE_SB_GetMsgId( msg ));
        }
        return ok;
    }
    bool Filter::allowed(const  CFE_SB_Msg_t* msg)
    {
        uint16_t apid;
        bool ok(false);
        apid = CFE_SB_GetMsgId( msg );
        apid &= APID_MASK;
        ok = allowedList_.at( apid );

        return ok;
    }
    bool Filter::ack_allowed(const  CFE_SB_Msg_t* msg)
    {
        uint16_t apid;
        bool ok(false);
        apid = CFE_SB_GetMsgId( msg );
        apid &= APID_MASK;
        ok = ackAllowedList_.at( apid );

        return ok;
    }
    bool Filter::allowed( const CFE_SB_MsgId_t id )
    {
        bool ok;
        CFE_SB_MsgId_t msgid = id & APID_MASK;
        ok = allowedList_.at( msgid );
        return ok;

    }
    int Filter::depth(const CFE_SB_MsgId_t id ) {
        uint16_t apid;
        apid = id & APID_MASK;
        int depth = depthTable_.at(apid);
        return depth;
    }
    bool Filter::decimated( const CFE_SB_Msg_t* msg )
    {
        size_t modCount;
        uint16_t apid;

        apid = CFE_SB_GetMsgId( msg );
        apid &= APID_MASK;
        modCount = decimationCounter_.at( apid ) % decimationRate_.at( apid ); 
        decimationCounter_.at( apid ) = decimationCounter_.at( apid )+1;
        return modCount == 0;
    }
    int Filter::routing( const CFE_SB_Msg_t* msg )
    {
        uint16_t apid;
        apid = CFE_SB_GetMsgId( msg );
        apid &= APID_MASK;
        int route;
        route = routingTable_.at( apid );
        return route;
    }

    void Filter::defaults()
    {
        for( size_t k=0 ; k < MAX_APID ; k++)
        {
            allowedList_[k] = false;
            ackAllowedList_[k] = true;
            decimationCounter_[k]=0;
            decimationRate_[k]=1;
            routingTable_[k]=0;
            depthTable_[k]=0;
        }
    }
    Filter::FilterStatus Filter::status( uint16_t apid )
    {
        Filter::FilterStatus o;
        o.allowed = allowedList_.at(apid);
        o.route   = routingTable_.at(apid);
        o.decimation = decimationRate_.at(apid);
        o.decimationCtr = decimationCounter_.at(apid) ;
        o.bufferDepth = depthTable_.at(apid);
        return  o;
    }
    uint16_t Filter::as_tlm_id( uint16_t apid )
    {
        return apid | SEC_HEADER_MASK;
    }

}