#pragma once
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <string>
#include <chrono>

namespace cromulence::network
{

    class UdpClient
    {
    public:
        UdpClient( uint16_t port, std::string ip ,std::chrono::microseconds us );
        ~UdpClient();

        bool sends( std::string message );
        std::string reads( );
        template<typename T> void send( const T &data )
        {
            
            sendto(socketFd_, &data,  sizeof(T), 
            MSG_CONFIRM, (const struct sockaddr *) &serverAddr_,  
            sizeof(serverAddr_)); 
        }
        template<typename T> bool read(  T& out)
        {
            struct sockaddr_in  clientAddr;
            socklen_t addrLen = sizeof(clientAddr);
            ssize_t rcvd = read( &out , sizeof(T));
            bool success;
            success = ( rcvd == sizeof(T));
            return success;
        }

    protected:
        ssize_t read( void* out , size_t sz );
        ssize_t send( const void* out , size_t sz );
        int socketFd_; 
        struct sockaddr_in serverAddr_;
        static const size_t MAX_BUF = 1024; 
        char buf_[MAX_BUF];
    };

}
