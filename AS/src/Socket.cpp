#include "Socket.h"
#include <unistd.h>
#include <iostream>

Socket::Socket(const Address& address, time_t seconds)
{
    socketFD = socket(address.hints.ai_family, address.hints.ai_socktype, address.hints.ai_protocol);
    
    if(seconds > 0)
    {
        //Wait a certain amount of seconds before socket times out.
        tv.tv_sec = seconds;
        //Setting to 0 microseconds
        tv.tv_usec  = 0;
        //Setting socket options
        setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof tv);
    }
         
    if(connect(socketFD, address.info->ai_addr, address.info->ai_addrlen) < 0)
    {
        throw;
    }
}

Socket::~Socket() 
{
    if (socketFD != -1) 
        close(socketFD);  
}

ssize_t Socket::receive(char *buffer, int length)
{
    //TODO Write custom exception
    if(socketFD == -1)
        throw;
    return ::recv(socketFD, buffer, length, 0);
}

ssize_t Socket::send(const char* buffer, int length) 
{
    //TODO Write custom exception
    if (socketFD == -1) 
        throw;
    return ::send(socketFD, buffer, length, 0);
}
