#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <chrono>
namespace cromulence::network
{

    class UdpServer
    {
    public:
        UdpServer( );
        ~UdpServer( );
        bool is_open( );
        void connect( uint16_t port , std::chrono::microseconds us );
        void disconnect( );
        bool sends( std::string in );
        std::string reads( );
        void fixed_client( char* clientIp, uint16_t port );
        void reply_mode( );
        template<typename T> bool send(const T& data)
        {
            return send( &data, sizeof(T));
        }
        template<typename T> bool read( T& out)
        {
            ssize_t sz ;
            sz = read( &out, sizeof(T) );
            //printf("RX: %d\n" ,sz);
            return sz == sizeof(T);
        }
    protected:
        bool send( const void* data, size_t size );
        ssize_t read( void* data, size_t max_size, bool replyToClient =false);
        bool open_;
        int sockFd_;
        struct sockaddr_in servAddr_;
        struct sockaddr_in clientAddr_;
        socklen_t clientLen_;
        static const size_t MAX_STR = 256;

        bool replyMode_;
    };
}