
#include <memory>
#include <string>
#include "Socket.h"
#include "SSLSocket.h"
#include "Address.h"
#include "ParsedUrl.h"

#define TIMEOUT 30
class GetRequest 
{

   unsigned state;
   bool gzipped, chunked, redirect;
   std::unique_ptr<Socket> socket;
   std::string buffer;
   char *endOfHeader;

public:
   //TODO Overload string to not care about what's inside
   GetRequest(const ParsedUrl& url, std::string message, const Address &address);

   char *getHeader();
 
   void parseRequest();

   ~GetRequest();
};