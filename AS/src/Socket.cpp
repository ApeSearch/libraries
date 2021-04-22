#include "../include/AS/Socket.h"
#include <unistd.h>
#include <iostream>
#include <errno.h>
#define QUEUESIZE 32

Socket::Socket(const Address& address, time_t seconds)
{
    socketFD = socket(address.hints.ai_family, address.hints.ai_socktype, address.hints.ai_protocol);

    if(socketFD < 0)
      //TODO
      throw "fail";
    
    if(seconds > 0)
    {
        //Wait a certain amount of seconds before socket times out.
        tv.tv_sec = seconds;
        //Setting to 0 microseconds
        tv.tv_usec  = 0;
        //Setting socket options
        if(setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof tv) == -1)
            //TODO
            throw "fail";

        if(setsockopt(socketFD, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof tv) == -1)
            //TODO
            throw "fail";
    }
         
    if(connect(socketFD, address.info->ai_addr, address.info->ai_addrlen) < 0)
    {
      //TODO
        throw "fail";
    }
}


Socket::Socket(const struct sockaddr_in &addr)
  {
  socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(socketFD < 0)
    //TODO
    throw "fail";
  
  if(connect(socketFD, (struct sockaddr *) &addr, sizeof(addr)) < 0)
  {
    perror("Issue with connect:" );
    printf("Errno: %d\n", errno);

    sleep(60u);    
    
    throw "fail";
  }
    //TODO
  }

Socket::Socket(int port)
{
    socketFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if(socketFD < 0)
        //TODO
        throw "fail";

    //Set socket options to reuse address and port
    int enable = 1;

    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) < 0)
        //TODO
        throw "fail";
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    //Use internet socket?
    addr.sin_family = AF_INET;
    //Let OS map it to the correct address
    addr.sin_addr.s_addr = INADDR_ANY;
    
    addr.sin_port = htons(port);

    //Bind socket
    if(bind(socketFD, (sockaddr *) &addr, sizeof(addr)) < 0)
        //TODO
        throw "fail";

    //Mark this socket as listening passively    
    if(listen(socketFD, QUEUESIZE) < 0)
        //TODO
        throw "fail";
}

Socket::Socket() : socketFD(-1){}

APESEARCH::unique_ptr<Socket> Socket::accept(struct sockaddr *addr, socklen_t *addrlen)
{
    int connectionFD = ::accept(socketFD, addr, addrlen);
	if(connectionFD < 0 )
	  throw "fail";

        APESEARCH::unique_ptr<Socket> socket( new Socket() );
        socket->socketFD = connectionFD;

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
        throw "fail";
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
