#include "Request.h"
#include <iostream>

#ifdef testing
   #include <memory>
   using std::unique_ptr;
   #include <string>
   using std::string;
#else
   #include "../AS/include/AS/unique_ptr.h"
   using APESEARCH::unique_ptr;
#endif


Request::Request() : buffer(65536, '\0'){}


char *Request::getHeader()
    {
    int bytesToWrite = 0;
    static char const * const endHeader = "\r\n\r\n";
    char const *place = endHeader;
    
    char *bufPtr, *bufEnd;
    bufEnd = bufPtr = &*buffer.begin();
    //TODO check string.end() 

    while( *place && (bytesToWrite = socket->receive(bufPtr, (&*buffer.end()) - bufPtr)) > 0 )
        {
        bufEnd += bytesToWrite;
        while(*place && bufPtr != bufEnd)
        (*place == *bufPtr++) ? ++place : place = endHeader;
        //TODO Some statistics tracking to see how many times this runs
        if ( bufPtr == &*buffer.end() )
        {
            unsigned sizeOfHeaderSoFar = buffer.size();
            //TODO implemnent resize for string.h
            buffer.resize( buffer.size() << 1 );
            bufPtr = bufEnd = &*buffer.begin() + sizeOfHeaderSoFar;
        }
        }
        //construct string based off of buffer call our pase header function
        // Reached the end of header
        
        std::cout << std::string(&*buffer.begin(), bufPtr) << std::endl;
        return *place ? nullptr : bufPtr;
    } 


Result Request::getReqAndParse(const char *urlStr)
{
    ParsedUrl url( urlStr );
    Address address(url.Host, "80");
    std::string getMessage = url.formRequest();
    int attempts = 0;
    Result res;
    while(attempts < MAXATTEMPTS)
    {
      try
      {
         unique_ptr<Socket> socket( 
            strcmp(url.Service, "http://") ? new Socket(address, 30) : new SSLSocket(address, 30) );
         socket->send( &getMessage.front(), getMessage.size() );
         
         char *endOfHeader = getHeader();
         if ( nullptr )
         res = parseHeader(endOfHeader);

         getBody();
         
         // Parse header
      }
      //TODO Add error catching functionality for errono in Socket and SSLSocket
      catch(...)
      {
         //std::cout << "send failed" << endl; 
         //TODO Update address to loop around back to the beginning
         if(address.info->ai_next)
            address.info = address.info->ai_next;
         else 
            address.info = address.head;
         ++attempts;
      }
    }

   return Result();
    
}  // end parseRequest()  

char * findString(char * begin, const char* end , const char *str )
   {
   char *place = str;
   while(*str && begin != end)
      (*place == *begin++) ? ++place : place = str;
   return begin;
   } // end findString

Request::parseHeader(endOfHeader)
{
   const char * newline = "\r\n";
   //Make it a c_str
   *endOfHeader = '\0';
   char *ptr = &*buffer.begin();

   char *endOfLine = findString( ptr, endOfHeader, newline ); // Finds up to \r\n

   

   

   scanf(ptr, "%s:%s/r/n", key, val);

}


void Request::getBody()
{
   
}

//TODO 
Request::~Request()
{

}
