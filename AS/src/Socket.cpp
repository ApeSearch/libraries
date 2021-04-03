#include "../include/AS/Socket.h"
#include <unistd.h>
#include <iostream>
#include <errno.h>
#define PORT 8080
#define QUEUESIZE 32

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
        if(setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof tv) == -1)
            //TODO
            throw;

        if(setsockopt(socketFD, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof tv) == -1)
            //TODO
            throw;
    }
         
    if(connect(socketFD, address.info->ai_addr, address.info->ai_addrlen) < 0)
    {
        throw;
    }
}


Socket::Socket(int port)
{
    socketFD = socket(AF_INET, SOCK_STREAM, 0);

    if(socketFD < 0)
        //TODO
        throw;

    //Set socket options to reuse address and port
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
        //TODO
        throw;
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    //Use internet socket?
    addr.sin_family = AF_INET;
    //Let OS map it to the correct address
    addr.sin_addr.s_addr = INADDR_ANY;
    
    addr.sin_port = htons(PORT);

    //Bind socket
    if(bind(socketFD, (sockaddr *) &addr, sizeof(addr)) < 0)
        //TODO
        throw;

    //Mark this socket as listening passively    
    if(listen(socketFD, QUEUESIZE) < 0)
        //TODO
        throw;
}

Socket::Socket() : socketFD(-1){}

Socket Socket::accept(struct sockaddr *addr, socklen_t *addrlen);
{
        int connectionFD = accept(socketFD, addr, addrlen);
        Socket socket();
        socket.socketFD = connectionFD;

        return socket;
}

Socket::~Socket() 
{
    if (socketFD != -1) 
        close(socketFD);  
}

void checkErrno();

ssize_t Socket::receive(char *buffer, int length)
{
    //TODO Write custom exception
    if(socketFD == -1)
       checkErrno();
       
    return ::recv(socketFD, buffer, length, 0);
}

ssize_t Socket::send(const char* buffer, int length) 
{
    //TODO Write custom exception
    if (socketFD == -1) 
    {
        throw;
    }
    int res = ::send(socketFD, buffer, length, 0);
    if( res == -1 )
        checkErrno();  
    return res;
}


void checkErrno()
   {
    switch( errno )
    {
    case EWOULDBLOCK:
        throw;
    default:
        throw;
    }
   }