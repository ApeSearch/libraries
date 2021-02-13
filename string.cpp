#include "string.h"

using nstd::string;

string::string() : buffer( nullptr ), size( 0 )
   {
   }

string::string( const string &s ) : size( s.size ), buffer( new char[ s.size ] )
   {
   for ( int i = 0;  i < size;  ++i )
      buffer[ i ] = s[ i ];
   } // end string(const string& s)

string::string( const char *s )
   {
   size = copy_cstr(buffer, s);
   } // end string(const string& s)

string::string( const char *s, size_t n ) : size( n ), buffer( new char[ n+1 ] )
   {
   nstd::strcpy( buffer, s );
   } // end string(const string *s, size_t n)

string::~string()
   {
   size = 0;
   delete[] buffer;
   } // end ~string()

//////////////////////
// MEMBER FUNCTIONS //
//////////////////////

size_t string::length() const
   {
   return size;
   } // end length()

const char *string::c_str()
   {
   return buffer;
   } // end c_str()

///////////////
// OPERATORS //
///////////////

string &string::operator=( const string &s )
   {
   size = s.length();
   if (buffer != nullptr)
       delete [] buffer;
   nstd::strcpy(buffer = new char[size+1], s.buffer);

   return *this;
   } // end operator =(const string& s)

string &string::operator=( const char *s )
    {
    if ( s != buffer )
        delete[] buffer;
    size = copy_cstr(buffer, s);

    return *this;
    } // end operator =(const string& s)

const char &string::operator[]( int pos ) const
   {
   return buffer[ pos ];
   } // end operator []()

char &string::operator[]( int pos )
   {
   return buffer[ pos ];
   } // end operator []()

std::ostream &nstd::operator<<( std::ostream &os, const string &str )
   {
   for ( int i = 0;  i < str.length();  ++i )
      os << str.buffer[ i ];
   return os;
   } // end operator <<()

bool nstd::operator==( const string &lhs, const string &rhs )
   {
   if ( lhs.length() != rhs.length() )
      return false;
   else
      {
      for ( int i = 0;  i < lhs.length();  ++i )
         {
         if ( lhs[ i ] != rhs[ i ] )
            return false;
         }
      }
   return true;
   } // end operator ==()

char* nstd::strcpy( char *dest, const char *src )
   {
   if (dest == nullptr)
      return nullptr;
   
   while ( (*dest++ = *src++) != '\0' )
   *dest = '\0';
   
   return dest;
   } // end strcpy( char *dest, const char *src )

char* nstd::strncpy( char *dest, const char *src, size_t num )
   {
   if (dest == nullptr)
      return nullptr;
   
   while (num-- && ((*dest++ = *src++) != '\0'))
   *dest = '\0';
   
   return dest;
   } // end strcpy( char *dest, const char *src )

int nstd::copy_cstr(char * & buffer, const char *s) {
    int i = 0;
    const char *ptr = s;
    while ( *ptr++ )
       ++i;
    buffer = new char[ i+1];
    nstd::strcpy( buffer, s );
    return i;
} // end copy_cstr(char * & buffer, const char *s)