#pragma once
#include "Socket.h"
#include <openssl/ssl.h>

class SSLSocket : public Socket
{
   public:
     SSLSocket(const Address& address, time_t seconds);
      
     ~SSLSocket();

     ssize_t send(const char* buffer, int length) override;

     ssize_t receive(char *buffer, int length) override;

   protected:
    // Pointerto to a framework which establishes TLS/SSL enabled ocnnections
      SSL_CTX * sslFramework;
      SSL * ssl;
};