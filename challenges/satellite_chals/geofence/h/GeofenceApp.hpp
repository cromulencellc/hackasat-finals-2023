#pragma once

#include "ASyncApp.hpp"
#include "GeofenceMessages.hpp"
#include "IoMessages.hpp"
#include "GpsMessages.hpp"
#include "Geofence.hpp"
namespace cromulence::geofence
{
    class GeofenceApp : public ASyncApp
    {
    public:
        GeofenceApp();
        ~GeofenceApp();
        void initialize();
        void shutdown();

        void noop( NoOpMsg &m);
        void reset( ResetMsg &m );
        void get_fences( GetMsg &m );
        void set_offsets( TimeOffsetsMsg &m);
        void load_fence( LoadGeofenceMsg &m);
        void snap_request( SnapRequestMsg &m);
        void team_change( TeamChangeMsg &m);
        void download_request( DownloadRequestMsg &m);
        void handle_orientation( moonlighter::io::AcbStateMsg &m);
        void handle_position( cromulence::gps::EcefMsg &m );
        void handle_polling( moonlighter::io::PollingConfigMsg &m);
       
    protected:
        static constexpr double ANTI_SPAM_INTERVAL = 5.0;
        void mark_time( );
        double seconds_since_mark( );
        double utc_secs( );
        std::chrono::time_point<std::chrono::system_clock> mark_;
        std::string image_path( uint8_t id );
        std::string defaultGeofence_;
        cromulence::Geofence fence_;
        bool blocked_;
        std::string team_;

        double qAxis_[3];
        double qS_;
        
        static constexpr double MEASURE_TIMEOUT = 20.0;
        double lastQTime_;
        double lastPosTime_;


    };
}