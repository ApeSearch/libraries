#include <string>
#pragma once

class ParsedUrl
   {
   public:
      const char *CompleteUrl;
      char *Service, *Host, *Port, *Path;
      bool protocolType; // true == http, false == https

      std::string formRequest() 
      {
         std::string header;
         header.reserve(50);
         // Get req
         header += "GET /";
         header.append(Path); 
         
         header += " HTTP/1.1\r\n";
         
         header += "Host: ";
         header.append(Host);
         header += "\r\nUser-Agent: LinuxGetUrl/ username@email.com\r\nAccept:";
         header += "*/*\r\nAccept-Encoding: identity\r\nConnection: close\r\n\r\n";
         return header;
      } // end formRequest()


      ParsedUrl( const char *url )
         {
         // Assumes url points to static text but
         // does not check.

         CompleteUrl = url;

         pathBuffer = new char[ strlen( url ) + 1 ];
         const char *f;
         char *t;
         for ( t = pathBuffer, f = url; *t++ = *f++; )
            ;

         Service = pathBuffer;

         const char Colon = ':', Slash = '/';
         char *p;
         for ( p = pathBuffer; *p && *p != Colon; p++ )
            ;

         if ( *p )
            {
            // Mark the end of the Service.
            *p++ = 0;
            protocolType = strcmp(Service, "https");

            if ( *p == Slash )
               p++;
            if ( *p == Slash )
               p++;

            Host = p;

            for ( ; *p && *p != Slash && *p != Colon; p++ )
               ;

            if ( *p == Colon )
               {
               // Port specified.  Skip over the colon and
               // the port number.
               *p++ = 0;
               Port = +p;
               for ( ; *p && *p != Slash; p++ )
                  ;
               }
            else
               Port = p;

            if ( *p )
               // Mark the end of the Host and Port.
               *p++ = 0;

            // Whatever remains is the Path.
            Path = p;
            }
         else
            Host = Path = p;
         }
         

      ~ParsedUrl( )
         {
         delete[ ] pathBuffer;
         }

   private:
      char *pathBuffer;
   };