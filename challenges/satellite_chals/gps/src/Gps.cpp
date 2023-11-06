#include "Gps.hpp"
using namespace std::chrono_literals;
namespace cromulence::gps
{

    GpsApp::GpsApp() : ASyncApp( 50 , "Gps" , 10s, EVT_ID ),
        requestCount_(BASE_REQUEST_COUNT),
        framesPerRequest_(DEFAULT_HEARTBEAT_FRAMES),
        frameCounter_(0),
        framesPerHk_(DEFAULT_HEARTBEAT_FRAMES),
        blocked_(false),
        pollingOn_(false)
    {
    }

    GpsApp::~GpsApp()
    {
    }

    void GpsApp::initialize()
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "App Online ");

        add_cmd_handler<NoOpMsg>( this, &GpsApp::noop );
        add_cmd_handler<ResetMsg>( this, &GpsApp::reset );
        add_cmd_handler<ConfigMsg>( this , &GpsApp::config );
        add_tlm_handler<moonlighter::io::NovatelOutMsg>( this, &GpsApp::raw_gps );
        add_tlm_handler<cromulence::heartbeat::PpsMsg>( this , &GpsApp::on_pps );
        frameCounter_ = 0;
        framesPerRequest_ = DEFAULT_HEARTBEAT_FRAMES;
        framesPerHk_ = DEFAULT_HEARTBEAT_FRAMES;
        requestCount_ = BASE_REQUEST_COUNT;
        blocked_ = false;
        pollingOn_ = false;
    }
    void GpsApp::shutdown()
    {

    }

    void GpsApp::noop(NoOpMsg &msg)
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "NoOp");

    }
    void GpsApp::reset(ResetMsg &msg)
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "Reset");
        frameCounter_ = 0;
        framesPerRequest_ = DEFAULT_HEARTBEAT_FRAMES;
        framesPerHk_ = DEFAULT_HEARTBEAT_FRAMES;
        requestCount_ = BASE_REQUEST_COUNT;
        blocked_ = false;
        pollingOn_ = false;
    }
    void GpsApp::config( ConfigMsg &msg )
    {
        pollingOn_ = (msg.data.payload.polling_enabled == 0) ? false : true;
        framesPerRequest_ = msg.data.payload.polling_rate;
        frameCounter_ = 0;
    }

    void GpsApp::raw_gps(moonlighter::io::NovatelOutMsg &msg)
    {
        p.to_message( msg.data.payload.response );
    }

    void GpsApp::on_pps( cromulence::heartbeat::PpsMsg &msg )
    {
        frameCounter_++;
        // Check if we should poll
        if((false == blocked_ ) &&
           (true == pollingOn_ ) &&
         (frameCounter_ % framesPerRequest_) == 0)
        {
            moonlighter::io::NovatelCmdMsg m;
            cromulence::strncpy_nt( m.data.payload.command ,"LOG BESTPOSA ONCE\r\n" , moonlighter::io::MAX_GPS_CMD_SIZE);
            m.data.payload.type = 1; // data comes back
            m.data.payload.id = requestCount_;
            requestCount_++;
            m.send();
        }
        // Check if we should send a housekeeping
        if( (frameCounter_ % framesPerHk_) == 0 )
        {
            // TODO HK Msg here.
        }
    }
    void GpsApp::polling_cfg( moonlighter::io::PollingConfigMsg &msg)
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "Polling config rxd %d", msg.data.payload.allowed );

        blocked_ = msg.data.payload.allowed == 0 ? true: false; 
    }

}