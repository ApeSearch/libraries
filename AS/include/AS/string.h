#include <cstddef> // for size_t
#include <iostream> //for ostream
#include <cstring>

#ifndef _NSTD_STRING_H
#define _NSTD_STRING_H

//TODO: Implement short-string optimization

#define NULLCHAR 1
#include <assert.h>
#include "algorithms.h"
#include <utility>

class string
   {
   public:  

      // Default Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates an empty string
      string( ) noexcept : string( size_t( 0 ) ) 
         {
         }
      //! A consequence for allowing the below constructor
      explicit string( char c ) noexcept : string( size_t( 1 ) )
         {
         *buffer = c;
         }
      
      // Gives a string with a buffer that remains undefined
      explicit string( size_t len ) noexcept : length ( len )
         {
         buffer = new char[length + NULLCHAR];
         *( buffer + length ) = '\0';
         }

      // string Literal / C string Constructor
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: Creates a string with equivalent contents to cstr
      string (const char* cstr ) noexcept : string( cstr, 0 ) {}
      
      // substring constructor
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: Creates a string with equivalent contents to cstr[pos] to cstr[pos + len]
      string (const char* cstr, size_t pos, size_t len = npos ) : length( len == npos ? strlen( cstr + pos ) : len ), 
               buffer( new char [ length + NULLCHAR ] )
         {
         assert( length <= strlen( cstr ) );
         // Captures a cast to string on comparison to empty c_string; implenting other relational operators is way, way better
         strncpy( buffer, cstr + pos, length ); // OK to use c-string functions bc cstr is passed in
         * ( buffer + length ) = '\0';
         }

      // string substring constructor
      // REQUIRES: cstr is a null terminated C style string, len <= s.length
      // MODIFIES: *this
      // EFFECTS: Creates a string with equivalent contents to cstr[pos] to cstr[pos + len]
      string ( const string& s, size_t pos, size_t len = npos ) noexcept : length( len == npos ? s.length - pos : len )
         {
         assert( length <= s.length && pos < s.length );
         buffer = new char [ length + NULLCHAR ];
         APESEARCH::copy( s.cbegin() + pos, s.cbegin() + pos + length, buffer );
         * ( buffer + length ) = '\0';
         }

      // copy Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: constructs a copy of str
      string ( const string& s ) noexcept : string( s, 0 ) {}
      
      // Move Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this, str
      // EFFECTS: Acquires the contents of str. str is left in an unspecified but valid state.
      string (string &&str) noexcept : string( ) 
         {
         swap( str );
         }

      string& operator=( string&& str )
         {
         // Do move operator
         swap( str );
         return *this;
         }
         
      // fill constructor
      // REQUIRES: nothing
      // MODIFIES: *this
      // EFFECTS: Fills the string with n consecutive copies of character c
      string (size_t n, char c) : length(n)
         {
         memset(buffer = new char [ length + NULLCHAR ], c, length);
         * ( buffer + length ) = '\0';
         }

      // string = operator
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: creates a string with contents equal to s
      string& operator=( const string &s )
         {
         string temp( s );
         swap ( temp );
         return *this;
         } // end operator =(const string& s)
      
      // string swapper
      // Swaps contents with another string
      void swap( string& other )
         {
         APESEARCH::swap( length, other.length );
         APESEARCH::swap( buffer, other.buffer );
         } // end swap

      // c-string = operator
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: creates a string with contents equal to s
      string& operator=( const char *s )
         {
         string temp ( s );
         swap (temp );
         return *this;
         } // end operator =(const string& s)

      ~string()
         {
         delete[] buffer;
         } // end ~string()

      // Size
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the number of characters in the string
      size_t size ( ) const
         {
         return length;
         }

      // Find
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns position of the first character of the first match
      size_t find (const string& other) const
         {
         // const char *a = buffer.cstr();
         const char *b = other.cstr();
         size_t i = 0, j = 0, start = 0;
         // bool found;
         while((*(buffer + j) != '\0') && (*(b + i) != '\0')){
            if(*(b + i) != *(buffer + j)){
               i = 0;
            }
            else{
               if(i == 0){
                  start = j;
               }
               ++i;
            }
            ++j;
         }
         return *(b + i) ? npos : start;
         }

      // C string Conversion
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a pointer to a null terminated C string of *this
      const char* cstr ( ) const
         {
         
         return buffer;
         }

      // Iterator Begin
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the start of the string
      char* begin ( ) 
         {
         // not 100% sure about begin() and end(), I believe cstr() is correct though as
         // the cppreference page explicitly says never to modify the return val of c_str()
         // implying it points to the internal buffer of the string
         return buffer;
         }

      const char * cbegin() const
         {
         return buffer;
         }

      // Iterator End
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the end of the string
      char* end ( ) 
         {
         return buffer + length;
         }

      const char* cend() const
         {
         return buffer + length;
         }

      // Element Access
      // REQUIRES: 0 <= i < size()
      // MODIFIES: Allows modification of the i'th element
      // EFFECTS: Returns the i'th character of the string
      char& operator [ ] ( size_t i )
         {
         return * ( buffer + i );
         }

      // string front
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a reference to the first character of the string.
      char& front() {
         return buffer[0];
      }

      // string front
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a const-qualified reference to the first character of the string.
      const char& front() const {
         return buffer[0];
      }

      // string Append
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Appends the contents of other to *this, resizing any
      //      memory at most once
      void operator+= ( const string& other )
         {
         string temp( length + other.size() );
         APESEARCH::copy( cbegin(), cbegin() + length, temp.buffer); // Copy original
         APESEARCH::copy( other.cbegin(), other.cend(), temp.buffer + length ); // Copy other string
         swap( temp ); // Swap contents
         }

      // Push Back
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Appends c to the string
      void push_back ( char c )
         {
         auto newbuf = new char[length + 2];
         strcpy(newbuf, buffer);
         newbuf[length] = c;
         newbuf[++length] = '\0';
         delete[] buffer;
         buffer = newbuf;
         }

      // Pop Back
      // REQUIRES: string is not empty
      // MODIFIES: *this
      // EFFECTS: Removes the last charater of the string
      void pop_back ( )
         {
         assert(length); // For our safety...
         buffer[--length] = '\0';
         //! Should the size be shrunk?
         }

      // Equality Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether all the contents of *this
      //    and other are equal
      bool operator== ( const string& other ) const
         {
         return !compare( other );
         }

      // Not-Equality Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether at least one character differs between
      //    *this and other
      bool operator!= ( const string& other ) const
         {
         return compare( other );
         }

      // Less Than Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less than other
      bool operator< ( const string& other ) const
         {  
         return compare( other ) < 0;
         }

      // Greater Than Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically greater than other
      bool operator> ( const string& other ) const
         {
         return compare( other ) > 0;
         }

      // Less Than Or Equal Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less or equal to other
      bool operator<= ( const string& other ) const
         {
         return compare( other ) <= 0;
         }

      // Greater Than Or Equal Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less or equal to other
      bool operator>= ( const string& other ) const
         {
         return compare( other ) >= 0;
         }

      char *findPtr( const string& substr ) const
         {
         const char * substr_c = substr.cstr(); // for substr
         char const *ptr = buffer; // to this string
         char const * const constend = cend();
         char const * const constSubstrEnd = substr.cend();
         while ( substr_c != constSubstrEnd && ptr != constend )
            *ptr++ == *substr_c ? ++substr_c : substr_c = substr.cstr();
         return ptr != constend ? buffer + ( ptr - buffer - substr.size() ) :  buffer + length;
         }
      int compare( const string& other ) const
         {
         const char *c_str1, *c_str2;
         const char * const str1End = cend();
         const char * const str2End = other.cend();
         c_str1 = buffer;
         c_str2 = other.buffer;
         for(; c_str1 != str1End && c_str2 != str2End 
            && *c_str1 == *c_str2; ++c_str1, ++c_str2 );
         
         if ( c_str1 == str1End )
            {
            if ( c_str2 != str2End )
               return -1;
            }
         else if ( c_str2 == str2End )
            return 1;

         return static_cast<int> ( *(const unsigned char*)c_str1 - *(const unsigned char*)c_str2 );
         } // end compare()

   static constexpr size_t npos = static_cast<size_t>( -1 );
   private:
      size_t length;
      char *buffer = nullptr;


   };

std::ostream& operator<< ( std::ostream& os, const string& s )
   {
   for ( const char *ptr = s.cbegin(), *const end = s.cend(); ptr != end; )
      os << *ptr++;
   return os;
   }

#endif
