#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include "UdpServer.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

namespace cromulence::network
{
    UdpServer::UdpServer(  ) : 
        open_(false)
        ,sockFd_(0)
        ,replyMode_(true)

    {
        memset(&servAddr_, 0, sizeof(servAddr_));
        memset(&clientAddr_,0,sizeof(clientAddr_));

    }

    
    UdpServer::~UdpServer( )
    {

    }
    
    bool UdpServer::is_open( )
    {
        return open_;
    }

    void UdpServer::reply_mode( )
    {
        replyMode_ = true; 
    }
    void UdpServer::connect( uint16_t port  , std::chrono::microseconds us )
    {
        sockFd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if( sockFd_ < 0 )
        {
            throw std::runtime_error("Bad UDP Server socket");
        }
        memset(&servAddr_, 0, sizeof(servAddr_));
        servAddr_.sin_family    = AF_INET; // IPv4
        servAddr_.sin_addr.s_addr = INADDR_ANY;
        servAddr_.sin_port = htons(port);
        //
        struct timeval tv;
        tv.tv_sec = us.count() / 1000000;
        tv.tv_usec = us.count() % 1000000;
        if (setsockopt(sockFd_, SOL_SOCKET, SO_RCVTIMEO  ,&tv,sizeof(tv)) < 0) {
            printf("Sock error");
        }
        int reuse = 1;

        if (setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        {
            perror("setsockopt(SO_REUSEADDR) failed");    
        
        }
        // now bind please 
        int b;
        b = bind(sockFd_, (const struct sockaddr *)&servAddr_, sizeof(servAddr_));
        if( b < 0 )
        {
            throw std::runtime_error("Bad UDP Bind");
        }

        // set to nonblock
        //int flags = fcntl(sockFd_, F_GETFL, 0);
        //fcntl(sockFd_, F_SETFL, flags | O_NONBLOCK);
        open_ = true;
        
    }
    void UdpServer::disconnect()
    {
        if( true == open_ )
        {
            close( sockFd_ );
        }
        open_ = false;
    }
    void UdpServer::fixed_client( char* clientIp,  uint16_t port )
    {
        clientAddr_.sin_family = AF_INET;
        clientAddr_.sin_port = htons(port);
        clientAddr_.sin_addr.s_addr = inet_addr(clientIp);
        memset(clientAddr_.sin_zero, '\0', sizeof(clientAddr_.sin_zero));
        clientLen_ = sizeof( clientAddr_ );
        replyMode_ = false;
    }
    bool UdpServer::send( const void* data, size_t size )
    {
        ssize_t sent;
        sent = sendto( sockFd_, data, size,MSG_CONFIRM, (struct sockaddr *)&clientAddr_, sizeof(sockaddr_in) ) ;

        return ( size == sent);
    }
    ssize_t UdpServer::read( void* data, size_t max_size, bool replyToClient)
    {
        struct sockaddr_in client;
        socklen_t len = sizeof( client );
        void *buff = reinterpret_cast<char*>( data );
        ssize_t n;
        n = recvfrom(sockFd_, buff , max_size, MSG_WAITALL, ( struct sockaddr *) &client, &len);
        if( (true == replyMode_ ) && (n >0) ) 
        {
            clientAddr_ = client;
            clientLen_  = len;
        }
        else
        {
        }
        return n;
    }
    bool UdpServer::sends( std::string in )
    {
        const void* ptr = reinterpret_cast<const void*>(in.c_str() );
        return send(ptr , in.length() );
    }
    std::string UdpServer::reads( )
    {
        char read_buffer[MAX_STR+1];
        ssize_t rd;
        memset( read_buffer, 0 , MAX_STR+1);
        rd = read( read_buffer , MAX_STR);

        if( rd <=  0 )
        {
            return "";
        }
        std::string out(read_buffer,rd);
        return out;
    }
}