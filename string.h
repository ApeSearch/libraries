
#ifndef _NSTD_STRING_H // include guard
#define _NSTD_STRING_H

#include <iostream>
#include <stdlib.h>

// not std kek
namespace nstd
   {

class string
   {
   public:
      // Constants
      static const size_t npos = -1;

      // Constructors/Destructor
      string();
      string( const string &s );
      string( const char *s );
      string( const char *s, size_t n );
      ~string();

      // Member Functions
      size_t length() const;
      const char *c_str();
      string substr( size_t pos, size_t len );

      // Operators
      string &operator=( const string &s );
      string &operator=( const char *s );
      char &operator[]( int pos );
      const char &operator[]( int pos ) const;

      friend std::ostream &operator<<( std::ostream &os, const string &str );
      friend bool operator==( const string &lhs, const string &rhs );

   private:
      size_t size;
      char *buffer;
   };

char* strcpy( char *dest, const char *src );
char* strncpy( char *dest, const char *src, size_t num );
int copy_cstr(char * & buffer, const char *s);
   } // namespace nstd
#endif