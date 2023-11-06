#pragma once

#include "ASyncApp.hpp"
#include "IoMessages.hpp"
#include "AttitudeMessages.hpp"
#include "HeartbeatMessages.hpp"
#include "Safe.hpp"
namespace cromulence::attitude
{

    class AttitudeApp : public cromulence::ASyncApp
    {
    public:
        AttitudeApp();
        ~AttitudeApp();

        void initialize();
        void shutdown();

        void noop(NoOpMsg &msg);
        void reset(ResetMsg &msg);
        void config( ConfigMsg &msg );
        void attitude_request( RequestMsg &msg );


        void on_heartbeat( cromulence::heartbeat::PpsMsg &msg );
        void attitude_tlm( moonlighter::io::AcbStateMsg &msg);
        void polling_tlm( moonlighter::io::PollingConfigMsg &msg);
    protected:
        void attitude_cmd( double qx, double qy, double qz, double qS );
        bool enabled_;
        bool blocked_;
        double maxBlind_;
        size_t counter_;
        size_t framesPerRequest_;
        Safe safe_;
        static constexpr double MAX_BLIND_DEFAULT=1000000000.0;
        static const size_t FRAMES=10;
        static constexpr const char* REQUEST_APP_NAME = "AttitudeReqeust";
    };
}
