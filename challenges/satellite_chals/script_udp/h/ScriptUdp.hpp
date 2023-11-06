#pragma once
#include "ASyncApp.hpp"
#include "IoMessages.hpp"
#include "UdpServer.hpp" 
#include "ScriptUdpMessages.hpp"
#include "GpsMessages.hpp"
#include <string>
namespace cromulence::script_udp
{

    void listener( std::string ip , uint16_t port);
    template<typename T> bool to_int( std::string in , T& out )
    {
        int i;
        constexpr T max_value {std::numeric_limits<T>::max()};
        constexpr T min_value {std::numeric_limits<T>::min()};
        try
        {
            i = std::stoi( in );
            if( (i >= min_value) && ( i <= max_value ) )
            {
                out = i;
                return true;
            }
        }
        catch(...)
        {
            return false;
        }
        return false; // 
    }
    class ScriptUdpApp : public ASyncApp
    {
        enum OpCode
        {
            GPS=0,
            SNAP,
            DOWNLOAD,
            KIDDIE,
            CAM_POWER,
        };
    public:
        ScriptUdpApp();
        ~ScriptUdpApp();

        void initialize();
        void shutdown();

        void noop( NoOpMsg &msg);
        void reset( ResetMsg &msg  );
        void configure( ConfigureMsg &msg );
        void gps_response( cromulence::gps::GeodeticMsg &msg  );
        void set_team( TeamMsg &msg);
        void processTimeout( );
    protected:
        void check_script( );
        void gps_position();
        void take_picture( );
        void download_picture();
        void script_kiddie();
        void camera_power();

        cromulence::network::UdpServer udp_;
        uint16_t gpsCount_;
        std::string currentTeam_;
        std::string downloadPath_;
        static const uint16_t SERVER_PORT = 9000;
        static const uint16_t CLIENT_PORT = 9001;
        static constexpr const char * CLIENT_IP = "127.0.0.1";
    };
}