#pragma once

#ifndef SOCKET_H_AS
#define SOCKET_H_AS

#include "Address.h"
#include "unique_ptr.h"
#include <sys/time.h> //timeval

class Socket 
{
   public:

      //Client Socket using hostname
      Socket(const Address& address, time_t seconds);

      //Server Socket
      Socket(int port);
      
      //Client socket using ip specified in addr
      Socket(const struct sockaddr_in &addr);

      Socket( Socket&& other ) : tv( other.tv ), socketFD( other.socketFD ) 
         { 
         other.socketFD = -1;
         }
      
      //TODO Why not we just check and close here?
      Socket& operator=( Socket&& other )
         {
         int ourFD = socketFD;
         socketFD = other.socketFD;
         other.socketFD = ourFD; // Was previously set to -1 but this Socket could've actually had a valid socket
         tv = other.tv;
         return *this;
         }

      //Default Constructor
      Socket();

      virtual ~Socket();

      APESEARCH::unique_ptr<Socket> accept(struct sockaddr *addr, socklen_t *addrlen);
      

      virtual ssize_t send(const char* buffer, int length);

      virtual ssize_t receive(char *buffer, int length);

      int getFD() const 
         {
         return socketFD;
         }

   protected:
      struct timeval tv;
      int socketFD;
};

#endif
