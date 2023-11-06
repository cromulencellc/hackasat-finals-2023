#pragma once
#include "ASyncApp.hpp"
#include "GpsMessages.hpp"
#include "IoMessages.hpp"
#include "NovatelSerializer.hpp"
#include "HeartbeatMessages.hpp"
namespace cromulence::gps
{

    class GpsApp : public cromulence::ASyncApp
    {
    public:
        GpsApp();
        ~GpsApp();

        void initialize();
        void shutdown();

        void noop( NoOpMsg &msg);
        void reset( ResetMsg &msg );
        void config( ConfigMsg &msg );

        void raw_gps( moonlighter::io::NovatelOutMsg &msg );
        void on_pps( cromulence::heartbeat::PpsMsg &msg );
        void polling_cfg( moonlighter::io::PollingConfigMsg &msg);
    protected:

        const size_t DEFAULT_HEARTBEAT_FRAMES = 10;
        const size_t BASE_REQUEST_COUNT = 1e6;
        size_t requestCount_;
        size_t framesPerRequest_;
        size_t frameCounter_;
        size_t framesPerHk_;
        bool blocked_;
        bool pollingOn_;
        NovatelDeSerializer p;

    };
}