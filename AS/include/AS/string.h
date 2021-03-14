#include <cstddef> // for size_t
#include <iostream> //for ostream

#ifndef _NSTD_STRING_H
#define _NSTD_STRING_H

//TODO: Implement short-string optimization

#define NULLCHAR 1
#include <assert.h>

char* strcpy( char *dest, const char *src );
char* strncpy( char *dest, const char *src, size_t num );
size_t strlen(const char *str);
int strcmp (const char *left, const char *right);

class string
   {
   public:  

      // Default Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates an empty string
      string( ) : length( 0 )
         {
         buffer = new char[NULLCHAR];
         *buffer = '\0';
         }

      // string Literal / C string Constructor
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: Creates a string with equivalent contents to cstr
      string (const char* cstr )
         {
         // Captures a cast to string on comparison to empty c_string; implenting other relational operators is way, way better
         length = strlen( cstr );
         buffer = new char [ length + NULLCHAR ];
         strncpy( buffer, cstr, length );
         * ( buffer + length ) = '\0';
         }
      
      // substring constructor
      // REQUIRES: cstr is a null terminated C style string
      // MODIFIES: *this
      // EFFECTS: Creates a string with equivalent contents to cstr[pos] to cstr[pos + len]
      string (const char* cstr, size_t pos, size_t len = npos )
         {
         // Captures a cast to string on comparison to empty c_string; implenting other relational operators is way, way better
         length = len;
         buffer = new char [ length + NULLCHAR ];
         strncpy( buffer, cstr + pos, length );
         * ( buffer + length ) = '\0';
         }

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
         int i = 0, j = 0, start = 0;
         // bool found;
         while((*(buffer + j) != '\0') && (*(b + i) != '\0')){
            // if(j == other.size()){
            //   found = true; 
            // }
            if(*(b + i) != *(buffer + j)){
               if(*(b + i) == '\0')
                  return start;

               ++j;
               i = 0;
            }
            else{
               if(i == 0){
                  start = j;
               }
               ++i;
               ++j;
            }
         }
         return -1;
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
      const char* begin ( ) const
         {
         // not 100% sure about begin() and end(), I believe cstr() is correct though as
         // the cppreference page explicitly says never to modify the return val of c_str()
         // implying it points to the internal buffer of the string
         return buffer;
         }

      // Iterator End
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the end of the string
      const char* end ( ) const
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

      // string Append
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Appends the contents of other to *this, resizing any
      //      memory at most once
      void operator+= ( const string& other )
         {
         char *newbuf = new char[length + other.size() + NULLCHAR];
         strncpy(newbuf, buffer, length);
         strncpy(newbuf + length, other.cstr(), other.size());
         length += other.size();
         * ( newbuf + length ) = '\0';
         delete[] buffer;
         buffer = newbuf;
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
         return strcmp(buffer, other.begin()) == 0;
         }

      // Not-Equality Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether at least one character differs between
      //    *this and other
      bool operator!= ( const string& other ) const
         {
         return strcmp(buffer, other.begin()) != 0;
         }

      // Less Than Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less than other
      bool operator< ( const string& other ) const
         {  
         return strcmp(buffer, other.begin()) < 0;
         }

      // Greater Than Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically greater than other
      bool operator> ( const string& other ) const
         {
         return strcmp(buffer, other.begin()) > 0;
         }

      // Less Than Or Equal Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less or equal to other
      bool operator<= ( const string& other ) const
         {
         return strcmp(buffer, other.begin()) <= 0;
         }

      // Greater Than Or Equal Operator
      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns whether *this is lexigraphically less or equal to other
      bool operator>= ( const string& other ) const
         {
         return strcmp(buffer, other.begin()) >= 0;
         }

      char *find( const string& substr ) const
         {
         const char * substr_c = substr.cstr(); // for substr
         char const *ptr = buffer; // to this string
         char const * const constend = end();
         while ( *substr_c && ptr != constend )
            *ptr++ == *substr_c ? ++substr_c : substr_c = substr.cstr();
         return ptr != constend ? buffer + ( ptr - buffer - substr.size() ) :  buffer + length;
         }
   private:
      size_t length;
      char *buffer;

      static const int npos = -1;
   };

std::ostream& operator<< ( std::ostream& os, const string& s )
   {
   for ( const char *ptr = s.begin(), *const end = s.end(); ptr != end; )
      os << *ptr++;
   return os;
   }

char* strcpy( char *dest, const char *src )
   {
   if (dest == nullptr)
      return nullptr;
   
   while ( (*dest++ = *src++) != '\0' );
   
   return dest;
   } // end strcpy( char *dest, const char *src )

char* strncpy( char *dest, const char *src, size_t num )
   {
   if (dest == nullptr)
      return nullptr;
   
   while (num-- && ((*dest++ = *src++) != '\0'));
   
   return dest;
   } // end strcpy( char *dest, const char *src )

size_t strlen(const char *str) 
   {
   size_t len = 0;
   while (*str++)
      len++;
   return len;
   }

int strcmp (const char *left, const char *right)
   {
   while (*left)
      {
         if (*left != *right)
            break;
         left++;
         right++;
      }
   return *left - *right;
   }


#endif