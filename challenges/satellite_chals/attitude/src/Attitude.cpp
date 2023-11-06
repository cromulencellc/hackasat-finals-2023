#include "Attitude.hpp"
#include <chrono>
using namespace std::chrono_literals;

namespace cromulence::attitude
{

    AttitudeApp::AttitudeApp() : ASyncApp( 50 , "Attitude" , 10s, EVT_ID ),
        enabled_(false),
        blocked_(false),
        maxBlind_( MAX_BLIND_DEFAULT ),
        counter_(0),
        framesPerRequest_(FRAMES)
    {

    }

    AttitudeApp::~AttitudeApp()
    {
        
    }
    void AttitudeApp::initialize()
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "Starting");
        maxBlind_ = MAX_BLIND_DEFAULT;
        counter_ = 0 ;
        framesPerRequest_ = FRAMES;
        enabled_ = false;
        blocked_ = false;
        add_cmd_handler<NoOpMsg>( this , &AttitudeApp::noop );
        add_cmd_handler<ResetMsg>( this , &AttitudeApp::reset );
        add_cmd_handler<ConfigMsg>( this , &AttitudeApp::config );
        add_cmd_handler<RequestMsg>( this , &AttitudeApp::attitude_request );

        //add_tlm_handler<moonlighter::io::AcbStateMsg>( this , &AttitudeApp::attitude_tlm );
        add_tlm_handler<moonlighter::io::PollingConfigMsg>( this, &AttitudeApp::polling_tlm );
        add_tlm_handler<cromulence::heartbeat::PpsMsg>( this , &AttitudeApp::on_heartbeat );
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "complete");

    }
    void AttitudeApp::shutdown()
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "shutting down");

    }
    void  AttitudeApp::noop(NoOpMsg &msg)
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "noop");

    }
    void  AttitudeApp::reset(ResetMsg &msg)
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "reset. polling disabled");
        maxBlind_ = MAX_BLIND_DEFAULT; 
        framesPerRequest_ = FRAMES;
        blocked_ = false;
        enabled_ = false;
        counter_ = 0 ;
    }
    void  AttitudeApp::config( ConfigMsg &msg )
    {
        CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "Configure");

        maxBlind_ = msg.data.payload.max_blind;
        enabled_ = msg.data.payload.enabled == 0 ? false : true;
        framesPerRequest_ = msg.data.payload.frame_count;

    }

    void  AttitudeApp::attitude_request( RequestMsg &msg )
    {
        if( ( true == enabled_ ) )
        {
            double qx = msg.data.payload.qAxis[0];
            double qy = msg.data.payload.qAxis[1];
            double qz = msg.data.payload.qAxis[2];
            double qs = msg.data.payload.qScalar;
            attitude_cmd( qx,qy,qz,qs);
        }
        else
        {
            CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "Blind %d , Enabled %d - ignoring pointing", false?1:0 , enabled_?1:0);

        }
    }

    void  AttitudeApp::attitude_tlm( moonlighter::io::AcbStateMsg &msg)
    {
        

        
        //blind_ = msg.data.payload.star_tacker_delay > maxBlind_;
        /*if((true == enabled_ ) && (false==blocked_) )
        {
            CFE_EVS_SendEvent(EVT_ID, CFE_EVS_INFORMATION, "Blinded - calculating safe quaternion" );

            cromulence::Gregorian utc;
            utc.yr = msg.data.payload.utc.yr;
            utc.mo = msg.data.payload.utc.mo;
            utc.day = msg.data.payload.utc.day;
            utc.hr = msg.data.payload.utc.hr;
            utc.min = msg.data.payload.utc.min;
            utc.sec = msg.data.payload.utc.sec;
            safe_.setTime( utc );
            Eigen::Quaterniond q;
            q = safe_.safe_quaternion();
            // TODO write that quaternion to a message
           // attitude_cmd( q.x() , q.y(), q.z() , q.w());
        }*/
    }
    void AttitudeApp::polling_tlm( moonlighter::io::PollingConfigMsg &msg )
    {
        blocked_ = msg.data.payload.allowed == 0 ? true: false; 
    }
    void AttitudeApp::attitude_cmd( double qx, double qy, double qz, double qS )
    {
        // tell moonlighter to do it nough
        moonlighter::io::AcbPointMsg o;
        o.data.payload.qAxis[0] = qx;
        o.data.payload.qAxis[1] = qy;
        o.data.payload.qAxis[2] = qz;
        o.data.payload.qScalar = qS;
        o.send();
        o.send(); // acb is spicy double send

        // also telemter is
        AttitudeCommandMsg t;
        t.data.payload.qAxis[0]=qx;
        t.data.payload.qAxis[1]=qy;
        t.data.payload.qAxis[2]=qz;
        t.data.payload.qScalar=qS;
        t.send();
    }

    void AttitudeApp::on_heartbeat( cromulence::heartbeat::PpsMsg &msg )
    {
        counter_++;

        if( ((counter_ % framesPerRequest_ ) == 0 ) &&
            (true == enabled_ )  &&
            (false == blocked_)) 
        {
            moonlighter::io::AcbGetTlmMsg m;
            m.send();
        }
    }
}
