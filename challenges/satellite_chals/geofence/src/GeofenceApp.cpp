#include "GeofenceApp.hpp"
#include "IoMessages.hpp"
#include "str.hpp"
using namespace std::chrono_literals;

namespace cromulence::geofence
{
    GeofenceApp::GeofenceApp() : ASyncApp(64, "Geofence", 10s, 100),
                                 blocked_(false),
                                 lastQTime_(0.0),
                                 lastPosTime_(0.0)
    {
    }
    GeofenceApp::~GeofenceApp()
    {
    }

    void GeofenceApp::initialize()
    {
        qAxis_[0] = 0.0;
        qAxis_[1] = 0.0;
        qAxis_[2] = 0.0;
        qS_ = 1.0;
        add_cmd_handler<NoOpMsg>(this, &GeofenceApp::noop);
        add_cmd_handler<ResetMsg>(this, &GeofenceApp::reset);
        add_cmd_handler<GetMsg>(this, &GeofenceApp::get_fences);
        add_cmd_handler<TimeOffsetsMsg>(this, &GeofenceApp::set_offsets);
        add_cmd_handler<LoadGeofenceMsg>(this, &GeofenceApp::load_fence);
        add_cmd_handler<SnapRequestMsg>(this, &GeofenceApp::snap_request);
        add_cmd_handler<TeamChangeMsg>(this, &GeofenceApp::team_change);
        add_cmd_handler<DownloadRequestMsg>( this, &GeofenceApp::download_request ) ;

        add_tlm_handler<moonlighter::io::AcbStateMsg>(this, &GeofenceApp::handle_orientation);
        add_tlm_handler<cromulence::gps::EcefMsg>(this, &GeofenceApp::handle_position);
        add_tlm_handler<moonlighter::io::PollingConfigMsg>(this, &GeofenceApp::handle_polling);
        //
        char truePath[256] = {};
        OS_TranslatePath("/cf/restricted.json", truePath);
        defaultGeofence_ = std::string(truePath);
        fence_.from_json(defaultGeofence_);
        blocked_ = false;
        mark_time();
    }

    void GeofenceApp::shutdown()
    {
    }

    void GeofenceApp::noop(NoOpMsg &m)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "No-op");
    }
    void GeofenceApp::reset(ResetMsg &m)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "reset: loading default geofence");
        fence_.from_json(defaultGeofence_);
        blocked_ = false;
        mark_time();
    }
    void GeofenceApp::get_fences(GetMsg &m)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Sending current geofence data down");
    }
    void GeofenceApp::set_offsets(TimeOffsetsMsg &m)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION,
                          "Setting time offsets to DAT: %f , DUT1: %f",
                          m.data.payload.leapseconds,
                          m.data.payload.deltaUT1);

        fence_.updateTimeOffsets(m.data.payload.leapseconds,
                                 m.data.payload.deltaUT1);
    }
    void GeofenceApp::load_fence(LoadGeofenceMsg &m)
    {
        char truePath[256];
        OS_TranslatePath(m.data.payload.pathname, truePath);
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Loading geofence at %s", truePath);
        fence_.from_json(truePath);
    }
    void GeofenceApp::snap_request(SnapRequestMsg &m)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Snap request");
        double markSec = seconds_since_mark();
        double curSec = utc_secs();
        if (false == blocked_)
        {
            bool spam = ( markSec < ANTI_SPAM_INTERVAL );
            bool oldQ = ( curSec - lastQTime_) > MEASURE_TIMEOUT;
            bool oldPos = ( curSec - lastPosTime_ ) > MEASURE_TIMEOUT;

            if( (false == spam ) && ( false == oldQ) && (false == oldPos) )
            {
                if (fence_.calcRestricted())
                {
                    // rejected
                    std::string zone = fence_.rejected_zone();
                    CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Snap rejected %s", zone.c_str());
                    RejectedMsg r;
                    strcpy(r.data.payload.name, zone.c_str());
                    r.data.payload.tai = fence_.getTAI();
                    // TODO - we want a timestamp so that we can say what got rejected
                    r.send();
                }
                else
                {
                    // not -rejected send a real snap
                    CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Snap accepted");
                    moonlighter::io::ImageSnapMsg s;
                    std::string imageName;
                    imageName = image_path( m.data.payload.image_id );
                    cromulence::strncpy_nt(s.data.payload.img, imageName.c_str(), moonlighter::io::MAX_IMG_NAME_SIZE);
                    s.send();
                    mark_time();
                    // hey it works lets meta data
                    SnapMetaMsg meta;
                    meta.data.payload.timeJd = fence_.getJdUtc();
                    Eigen::Vector3d gps = fence_.getGps();
                    meta.data.payload.gpsPosition[0] = gps[0];
                    meta.data.payload.gpsPosition[1] = gps[1];
                    meta.data.payload.gpsPosition[2] = gps[2];
                    meta.data.payload.qAxis[0] = qAxis_[0];
                    meta.data.payload.qAxis[1] = qAxis_[1];
                    meta.data.payload.qAxis[2] = qAxis_[2];
                    meta.data.payload.qScalar = qS_;
                    cromulence::strncpy_nt(meta.data.payload.team, team_.c_str(), TEAM_NAME_SIZE);
                    meta.send();
                }
            }
            else
            {
                CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Rejected - spam: %d, pos: %d, q: %d ", 
                                spam ?1:0 , 
                                oldPos?1:0,
                                oldQ?1:0);
            }
        }
        else
        {
            CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Rejected - blocking");
        }
    }
    void GeofenceApp::handle_position(cromulence::gps::EcefMsg &m)
    {
        // VULN: Using raw gps receiver data instead of filtered data from the adcs
        // what time is it
        lastPosTime_ = utc_secs();
        // set the position up
        fence_.setGpsPosition(m.data.payload.position );
    }
    void GeofenceApp::handle_orientation(moonlighter::io::AcbStateMsg &m)
    {
        lastQTime_ = utc_secs();
        // setup the time
        Gregorian t;
        t.yr = m.data.payload.utc.yr + 1900;
        t.mo = m.data.payload.utc.mo;
        t.day = m.data.payload.utc.day;
        t.hr = m.data.payload.utc.hr;
        t.min = m.data.payload.utc.min;
        t.sec = m.data.payload.utc.sec +
                (static_cast<double>(m.data.payload.utc.csec) / 100.0);
        fence_.setTime(t);
        
        qS_ = m.data.payload.qScalar;
        qAxis_[0] = m.data.payload.qAxis[0];
        qAxis_[1] = m.data.payload.qAxis[1];
        qAxis_[2] = m.data.payload.qAxis[2];
        fence_.setLookQuaternion(qS_,
                                 qAxis_);
    }

    void GeofenceApp::download_request( DownloadRequestMsg &m)
    {
        CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Attempting to download image %d", m.data.payload.image_id );

        if( false == blocked_ )
        {
            double sec = seconds_since_mark();

            if( sec > ANTI_SPAM_INTERVAL)
            {
                moonlighter::io::ImageGetMsg om;
                std::string image;
                image = image_path( m.data.payload.image_id );
                std::string dlpath;
                dlpath = fence_.download_path();
                cromulence::strncpy_nt( om.data.payload.img , image.c_str() , moonlighter::io::MAX_IMG_NAME_SIZE);
                cromulence::strncpy_nt( om.data.payload.download_directory ,dlpath.c_str() ,moonlighter::io::MAX_IMG_NAME_SIZE );
                om.send();
                CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "success");
                mark_time();
            }
            else
            {
                 CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Rejected - too fast");
            }

        }
        else
        {
            CFE_EVS_SendEvent(1, CFE_EVS_INFORMATION, "Rejected - blocking");

        }
    }
    void GeofenceApp::team_change(TeamChangeMsg &m)
    {
        team_ = std::string(m.data.payload.name);

        CurrentTeamMsg out;
        cromulence::strncpy_nt( out.data.payload.name , m.data.payload.name , TEAM_NAME_SIZE);
        out.send();
    }
    void GeofenceApp::handle_polling(moonlighter::io::PollingConfigMsg &m)
    {
        blocked_ = ( m.data.payload.allowed == 1 ) ? false: true;
    }
    std::string GeofenceApp::image_path( uint8_t id )
    {
        std::string path;
        path = team_ + "_" + std::to_string(id) + ".jpg";
        return path;
    }
    void GeofenceApp::mark_time( )
    {
        mark_ =  std::chrono::system_clock::now();
    }
    double GeofenceApp::seconds_since_mark()
    {
        auto current = std::chrono::system_clock::now();
        double secs = std::chrono::duration<double>(current-mark_).count();
        return  secs ;
    }
    double GeofenceApp::utc_secs()
    {
                // what time is it
        auto t = std::chrono::system_clock::now();
        std::chrono::duration<double>  t_utc = t.time_since_epoch();
        return t_utc.count();

    }
}