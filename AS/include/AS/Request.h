
#include <memory>
#include <string>
#include "Socket.h"
#include "SSLSocket.h"
#include "Address.h"
#include "ParsedUrl.h"

#define TIMEOUT 30
class Request 
{

enum class getReqStatus
{
   successful, // 
   redirected, // What url to redirected ( give to frontier for them to check )
   timedOut    //  
};

// Tells the crawler status of request
struct Result
{
   string url; // For redirects...
   getReqStatus status; 
};


   unsigned state;
   bool gzipped, chunked, redirect;
   std::unique_ptr<Socket> socket;
   std::string buffer;
   char *endOfHeader;

public:
   //TODO Overload string to not care about what's inside
   Request(const ParsedUrl& url, std::string message, const Address &address);

   char *getHeader();

   Result parseHeader();

   void getBody();

   ~Request();
};