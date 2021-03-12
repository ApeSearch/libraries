#include "GetRequest.h"
#include <iostream>
GetRequest::GetRequest(const ParsedUrl& url, std::string message,const Address &address) : buffer(65536, '\0'), 
socket(strcmp(url.Service, "http://") ? new Socket(address, 30) : new SSLSocket(address, 30))
{
    socket->send(message.c_str(), message.length());
}


char *GetRequest::getHeader()
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
        return bufPtr;
    } 

void GetRequest::parseRequest()
{
    char *endOfHeader = getHeader();
    
    
    // Once done
    // Parse
}  // end parseRequest()  

//TODO 
GetRequest::~GetRequest()
{

}
