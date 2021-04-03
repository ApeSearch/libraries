#pragma once

#ifndef SOCKET_H_AS
#define SOCKET_H_AS

#include "Address.h"
#include "unique_ptr.h"
#include <sys/time.h> //timeval

class Socket 
{
   public:

      //Client Socket
      Socket(const Address& address, time_t seconds);

      //Server Socket
      Socket(int port);

      //Default Constructor
      Socket();

      virtual ~Socket();

      APESEARCH::unique_ptr<Socket> accept(struct sockaddr *addr, socklen_t *addrlen);
      

      virtual ssize_t send(const char* buffer, int length);

      virtual ssize_t receive(char *buffer, int length);

   protected:
      struct timeval tv;
      int socketFD;
};

#endif