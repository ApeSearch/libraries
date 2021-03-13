#pragma once
#include "Address.h"
#include <sys/time.h> //timeval

class Socket 
{
   public:
      Socket(const Address& address, time_t seconds);
      
      virtual ~Socket();

      virtual ssize_t send(const char* buffer, int length);

      virtual ssize_t receive(char *buffer, int length);

   protected:
      struct timeval tv;
      int socketFD;
};