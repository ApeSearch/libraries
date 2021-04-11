
#pragma once

#ifndef AS_FILE_H
#define AS_FILE_H

#include <cassert>
#include <cstring>
#include <cstdint>
#include <exception>
#include <cerrno> // for errno
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // for close() file

namespace APESEARCH
{
    // A RAII for Files
    class File
       {
       int fd;
       friend class failure;
    public:

       class failure : public std::exception
          {
          friend class File;
       protected:
          int error_number;
          int error_offset;
          std::string error_message;

       public:
          explicit failure( const std::string& msg, int err_num, int err_off )
             : error_number( err_num ), error_offset( err_off ), error_message( msg ) {}

          explicit failure( const std::string&& msg, int err_num, int err_off )
             : error_number( err_num ), error_offset( err_off ), error_message( std::move( msg ) ) {}

          virtual ~failure() throw () {}

          virtual const char *what() const throw()
             {
             return error_message.c_str();
             }

          virtual int getErrorNumber() const throw() 
             {
             return error_number;
             }

          virtual int getErrorOffset() const throw()
             {
             return error_offset;
             }
          };

       File( ) : fd( -1 ) {}

       File( const char *pathname, int flags, mode_t mode ) : fd( open( pathname, flags, mode ) )
          {
          if ( fd == -1 )
             {
             perror("Error opening file");
             fprintf( stderr, "%s\n", pathname );
             throw failure( "Issue with opening file", errno, 0 );
             } // end if
          }  // end File

       File( const char *pathname, int flags ) : fd( open( pathname, flags ) )
          {
          if ( fd == -1 )
             {
             perror("Error opening file");
             fprintf( stderr, "%s\n", pathname );
             throw failure( "Issue with opening file", errno, 0 );
             } // end if
          }
      
      // The main purpose of this constructor is to encapsulate a file's lifetime in an object
       File( int _fd, int flags = 0, mode_t mode = 0600 ) : fd( _fd  ) {}
       // More efficent than std::swap
       File( File&& file ) : fd( file.fd )
          {
          
          file.fd = -1;
          }
      
      ssize_t write( const char *buf, size_t nbyte )
         {
         ssize_t bytesWritten = 0;
         ssize_t revd = 0;
         const char *end = buf + nbyte;
         while ( buf != end )
            {
            revd = ::write( fd, buf, nbyte );

            if ( revd < 0 )
               {
               perror("Error with writing to file");
               throw failure( "Issue iwth writing to file", errno, 0 );
               } // end if

            bytesWritten += revd;
            nbyte -= revd;
            buf += revd;
            }
         return bytesWritten;
         }
      
      ssize_t read( char *buf, size_t nbyte )
         {
         ssize_t bytesRead = 0;
         ssize_t revd = 0;
         while( nbyte && ( revd = ::read( fd, buf, nbyte ) ) > 0 )
            {
            
            bytesRead += revd;
            nbyte -= revd;
            buf += revd;
            } // end while
         if ( revd < 0 )
            {
            perror("Error with writing to file");
            throw failure( "Issue with writing to file", errno, 0 );
            } // end if
            
         return bytesRead;
         }

      ssize_t truncate(size_t length){
         ftruncate(fd, length);
      }

       File& operator=( File&& file )
          {
          #ifdef LOCAL
                using APESEARCH::swap;
          #else
                using std::swap;
          #endif
          swap( fd, file.fd );
          return *this;
          }

       // Const Operator and constructor or not allowed
       File( const File& ) = delete;
       File& operator=( const File& ) =delete;

       ~File()
          {
          if ( fd != -1 )
             close( fd );
          } // end ~File()

       inline int getFD() const
          {
          return fd;
          }
       };
} // end namespace AS

#endif // end AS_FILE_H