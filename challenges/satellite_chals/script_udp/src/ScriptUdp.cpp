#include "ScriptUdp.hpp"
#include <cstring>
#include <string>
#include "str.hpp"
#include "GpsMessages.hpp"
#include "GeofenceMessages.hpp"
namespace cromulence::script_udp
{

    using namespace std::chrono_literals;


    ScriptUdpApp::ScriptUdpApp() :  ASyncApp( 256,  "ScriptUdp" , 1s )
        ,gpsCount_(0)
        ,currentTeam_("noone")
    {

    }
    ScriptUdpApp::~ScriptUdpApp()
    {

    }
    void ScriptUdpApp::processTimeout( )
    {
        check_script();
    }
    void ScriptUdpApp::initialize()
    {
        add_cmd_handler<NoOpMsg>(this, &ScriptUdpApp::noop); 
        add_cmd_handler<ResetMsg>(this, &ScriptUdpApp::reset); 
        add_cmd_handler<ConfigureMsg>(this, &ScriptUdpApp::configure); 
        add_cmd_handler<TeamMsg>( this, &ScriptUdpApp::set_team);
        // tlm handler
        //add_tlm_handler<cromulence::gps::GeodeticMsg >(this, &ScriptUdpApp::gps_response ); 
        udp_.connect( SERVER_PORT, 1s );
        udp_.reply_mode();
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, " Script UDP online");

    }
    void ScriptUdpApp::shutdown()
    {

    }
    void ScriptUdpApp::noop( NoOpMsg &msg )
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, " Noop");

    }
    void ScriptUdpApp::reset( ResetMsg &msg)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, " Reset");

        if( true == udp_.is_open() )
        {
            udp_.disconnect();
        }
        udp_.connect(SERVER_PORT, 1s);
        udp_.reply_mode();
        gpsCount_ = 0;
        downloadPath_ = "/tmp/downloads";

    }
    void ScriptUdpApp::configure( ConfigureMsg &msg )
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, " Configure Server: %d", msg.data.payload.server_port);

        if( true == udp_.is_open( ))
        {
            udp_.disconnect();
        }
        udp_.connect( msg.data.payload.server_port, 1s);
        udp_.reply_mode();
    }

    void ScriptUdpApp::gps_response( cromulence::gps::GeodeticMsg &msg   )
    {
        udp_.send( msg.data.payload);
    }
    void ScriptUdpApp::check_script( )
    {
        uint8_t count=0;
        uint8_t opcode(100);
        // do this as long as we've go tmsgs
        while( true == udp_.read( opcode ) )
        {
            switch( opcode )
            {
                case OpCode::GPS:
                    gps_position( );
                    break;
                case OpCode::SNAP:
                    take_picture( );
                    break;
                case OpCode::DOWNLOAD:
                    download_picture();
                    break;
                case OpCode::KIDDIE:
                    script_kiddie();
                    break;
                case OpCode::CAM_POWER:
                    camera_power();
                    break;
                default:
                    //do nothing
                    break;
            }
            count++;
            if( count > 5 )
            {
                break; // dont spam me bro
            }
        }
    }

    void ScriptUdpApp::gps_position( )
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Script requested a gps measurement");

        std::string typeOfLog = udp_.reads( );
        moonlighter::io::NovatelCmdMsg msg;
        // VULN ADDED: This allows users to do a script injection attack
        snprintf( msg.data.payload.command , moonlighter::io::MAX_GPS_CMD_SIZE, "LOG BEST%sA ONCE\r\n", typeOfLog.c_str() );
        msg.data.payload.type = 1;//log=1
        msg.data.payload.id = gpsCount_;
        msg.send();
        gpsCount_++;
    }

    void ScriptUdpApp::take_picture()
    {
        std::string imdId;
        uint8_t id;
        bool ok;
        imdId = udp_.reads(  );
        ok  = to_int<uint8_t>( imdId , id );

        if( true == ok )
        {        
            CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Script requested a picture");

            cromulence::geofence::SnapRequestMsg msg;
            msg.data.payload.image_id = id;
            msg.send();

        }
        else
        {
            CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Bad image request");

        }

    }
    void ScriptUdpApp::camera_power()
    {
         CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Power camera");

        std::string pow;
        pow = udp_.reads();
        moonlighter::io::DevicePowerMsg m;
        if( 0 == pow.compare("on") )
        {
            m.data.payload.device = moonlighter::io::CAMERA_POWER;
            m.data.payload.power = 1;
            m.send();
        }
        else if( 0 == pow.compare("off") )
        {
            m.data.payload.device = moonlighter::io::CAMERA_POWER;
            m.data.payload.power = 1;
            m.send();
        }
        else
        {
            // otherwise ignore
        }
    }
    void ScriptUdpApp::script_kiddie()
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Script kiddies");

        // flag for script kiddies
        std::string msg;
        msg = udp_.reads( );
        // Not a vuln - just send the script kiddie message
        KiddieMsg m;
        cromulence::strncpy_nt( m.data.payload.team , currentTeam_.c_str() , TEAM_SIZE );
        cromulence::strncpy_nt( m.data.payload.message, msg.c_str( ), MESSAGE_SIZE );

        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "%s: %s", currentTeam_.c_str() , msg.c_str() );

        m.send();
         
    }
    void ScriptUdpApp::download_picture()
    {
        std::string imdId;
        uint8_t id;
        bool ok;
        imdId = udp_.reads(  );
        ok  = to_int<uint8_t>( imdId , id );
        if( true == ok )
        {        
            CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Script requested a download");

            cromulence::geofence::DownloadRequestMsg msg;
            msg.data.payload.image_id = id;
            msg.send();
        }
        else
        {
            CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Bad script download");

        }
    }

    void ScriptUdpApp::set_team( TeamMsg &msg)
    {
        char tmp[256]=  {};
        cromulence::strncpy_nt( tmp, msg.data.payload.name  , 256 );
        currentTeam_ = std::string(tmp);
    }
    
}