#pragma once
#ifndef VECTOR_H_APESEARCH
#define VECTOR_H_APESEARCH

#include "assert.h"
#include <unistd.h>
#include <utility>
#include <iostream>

#define DEFAULT_BUCKET_SIZE 8

static inline size_t computeTwosPowCeiling(size_t num) 
   {
   size_t powerNum = 1;
   for (; num; num >>=1 )
      powerNum <<= 1;
   return powerNum;
   }

namespace APESEARCH
{

template<typename T>
class vector
   {
   public:

      // Default Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs an empty vector with capacity 0
      vector ( ) : vector ( 0 )
         {
         }

      // Destructor
      // REQUIRES: Nothing
      // MODIFIES: Destroys *this
      // EFFECTS: Performs any neccessary clean up operations
      ~vector ( )
         {
         //delete [] _elts;
         free( _elts );
         }

      // Resize Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs a vector with size num_elements,
      //    all default constructed
      // ! It is okay to call new[0] but dlete must also be done
      vector ( size_t num_elements ) : _capacity( num_elements ? computeTwosPowCeiling(num_elements) : 0 )
            , _size( num_elements ) ,_elts( ( T *) malloc( sizeof( T ) * _capacity ) )
         {
         }

      // Fill Constructor
      // REQUIRES: Capacity > 0
      // MODIFIES: *this
      // EFFECTS: Creates a vector with size num_elements, all assigned to val
      vector ( size_t num_elements, const T& val ) : _capacity( num_elements ? computeTwosPowCeiling(num_elements) : 0 )
            , _size( num_elements ) ,_elts( ( T * ) malloc( sizeof( T ) * _capacity ) )
         {
         for (T *ptr = _elts, * const end = _elts + _size; ptr != end; )
              *ptr++ = val;
         }

      // Copy Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates a clone of the vector other
      vector ( const vector& other ) : _capacity( other._capacity ), _size( other._size ), _elts( ( T* ) malloc( sizeof( T ) * _capacity ) )
         {
         for (T *ptr = _elts, *otherPtr = other._elts, * const end = _elts + _size; ptr != end; )
            *ptr++ = *otherPtr++;
         }

      // Assignment operator
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Duplicates the state of other to *this
      vector& operator= ( const vector<T>& other )
         {
         // Doesn't check other != *this since it's the programmer's responsiblity to make sure such isn't the case
         vector<T> temp( other ); // utilize copy constructor
         swap( temp );
         return *this;
         }

      void swap( vector<T> &other ) 
         {
         swap( _elts, other._elts );
         swap( _size, other._size );
         swap( _capacity, other._capacity );
         }

      template<typename Type> void swap( Type&& itemOne, Type&& itemTwo )
         {
         swap( std::forward<Type>(itemOne), std::forward<Type>(itemTwo) );
         }
      template<typename Type> void swap( Type& itemF, Type& itemS )
         {
         Type temp( itemF );
         itemF =  itemS;
         itemS =  temp;
         } 
      // Move Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves other in a default constructed state
      // EFFECTS: Takes the data from other into a newly constructed vector
      vector ( vector&& other ) : _capacity( other._capacity  ), _size( other._size  ), _elts( other._elts  )
         {
         other._elts = nullptr; // To ensure no double deletes
         other._capacity = 0;
         other._size = 0;
         }

      // Move Assignment Operator
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves otherin a default constructed state
      // EFFECTS: Takes the data from other in constant time
      vector& operator= ( vector&& other )
         {
         swap( other );
         return *this;
         }

      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void reserve ( size_t newCapacity )
         {
         if (newCapacity <= _capacity) return;

         //T* newElements = new T[newCapacity];
         T *newElements = ( T* ) malloc( sizeof( T ) * newCapacity );

         for (T *ptr = newElements, *otherPtr = _elts, * const end = ptr + _size; ptr != end; )
            *ptr++ = std::move( *otherPtr++ );

         free( _elts );
         _elts = newElements;
         _capacity = newCapacity;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the number of elements in the vector
      size_t size() const
         { 
            return _size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns the maximum size the vector can attain before resizing
      size_t capacity() const
         {
            return _capacity;
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Allows modification of data[i]
      // EFFECTS: Returns a mutable reference to the i'th element
      T& operator[ ] ( size_t i )
         {
         assert (i >= 0 && i < _size);
         return *( _elts + i );
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Nothing
      // EFFECTS: Get a const reference to the ith element
      const T& operator[ ] ( size_t i ) const
         {
         assert (i >= 0 && i < _size);
         return *( _elts + i );
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size(), capacity()
      // EFFECTS: Appends the element x to the vector, allocating
      //    additional space if neccesary
      void push_back ( const T& x )
         {
         if (_size == _capacity)
            {
            reserve( _capacity ? _capacity << 1 : DEFAULT_BUCKET_SIZE );
            }
         *( _elts + _size++ ) = x;
         }
      
      // vector.emplace_back( std::forward<Args>( args )... );
      template< class ...Args>
      T& emplace_back ( Args&& ...args )
         {
         if (_size == _capacity)
            {
            reserve( _capacity ? _capacity << 1 : DEFAULT_BUCKET_SIZE );
            }
         return (*( _elts + _size++ ) = std::move( T( std::forward<Args>( args )... ) ) );
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size()
      // EFFECTS: Removes the last element of the vector,
      //    leaving capacity unchanged
      void pop_back ( )
         {
         //if (_size == 0) return;
         (_elts + --_size)->~T();
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to the 
      //    first element of the vector
      T* begin ( )
         {
         return _elts;
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to 
      //    one past the last valid element of the vector
      T* end ( )
         {
         return _elts + _size;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to the first element of the vector
      const T* begin ( ) const
         {
         return _elts;
         }

      // REQUIRES: Nothing
      // MODIFIES: Nothing
      // EFFECTS: Returns a random access iterator to 
      //    one past the last valid element of the vector
      const T* end ( ) const
         {
         return _elts + _size;
         }
      
      inline T& front()
         {
         return *_elts;
         }
      inline const T& front() const
         {
         return *_elts;
         }
      
      inline T& back()
         {
         assert( _size );
         return _elts[ _size - 1 ];
         }
      inline const T& backc() const
         {
         assert( _size );
         return _elts[ _size - 1 ];
         }

   private:

      size_t _capacity;
      size_t _size;
      T* _elts;





   /*
      void swap( vector<T> &other ) 
         {
         swap( _elts, other._elts );
         swap( _size, other._size );
         swap( _capacity, other.capacity );
         }

      template<typename Type> void swap( Type& itemF, Type& itemS )
         {
         T temp( std::move( itemF ) );
         itemF = std::move( itemS );
         itemS = std::move( temp );
         } 
   */
   };
   
} // end namespace APESEARCH
#endif