#include "UdpClient.hpp"
#include <chrono>

namespace cromulence::network
{
    UdpClient::UdpClient( uint16_t port , std::string ip, std::chrono::microseconds us )
    {
        socketFd_=  socket(AF_INET, SOCK_DGRAM , 0);
        if ( socketFd_ < 0 )
        { 
            throw("Whatever!");
        } 

        memset(&serverAddr_, 0, sizeof(serverAddr_)); 

        // Filling server information 
        serverAddr_.sin_family = AF_INET; 
        serverAddr_.sin_port = htons(port); 
        serverAddr_.sin_addr.s_addr =  inet_addr( ip.c_str() ); 
        struct timeval tv;
        tv.tv_sec = us.count() / 1000000;
        tv.tv_usec = us.count() % 1000000;
        if (setsockopt(socketFd_, SOL_SOCKET, SO_RCVTIMEO  ,&tv,sizeof(tv)) < 0) {
            printf("Sock error");
        }
    }
    UdpClient::~UdpClient()
    {
        close( socketFd_ );
    }

    bool UdpClient::sends( std::string msg )
    {

        ssize_t out;
        out = send( msg.c_str() , msg.length() );
        return out == msg.length();
    }
    std::string UdpClient::reads() 
    {
        ssize_t rcv;
        memset( buf_ , 0 , MAX_BUF);
        rcv = read( buf_ , MAX_BUF-1);
        if( rcv <= 0 )
        {
            return "";
        }
        std::string out( buf_ ,rcv);
        return out;
    }
    ssize_t UdpClient::send( const void* in, size_t sz )
    {
        size_t out;
        out = sendto(socketFd_,in, sz, MSG_CONFIRM, (const struct sockaddr *) &serverAddr_,  sizeof(serverAddr_)); 

        return out;
    }
    ssize_t UdpClient::read( void* out , size_t sz )
    {
        struct sockaddr_in  rxAddr;
        socklen_t addrLen;
        ssize_t recvLen = recvfrom(socketFd_, out, sz , MSG_WAITALL, (struct sockaddr *) &rxAddr, &addrLen);
        return recvLen;
    }


}
