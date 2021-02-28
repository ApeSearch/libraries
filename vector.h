#include "assert.h"
#include <unistd.h>

#ifndef _NSTD_STRING_H
#define _NSTD_STRING_H

#define BUCKET_SIZE 8

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
            delete [] _elts;
         }

      // Resize Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Constructs a vector with size num_elements,
      //    all default constructed
      // ! It is okay to call new[0] but dlete must also be done
      vector ( size_t num_elements ) : _size( num_elements ), _capacity( num_elements ), _elts(new T[ num_elements ]())
         {
         }

      // Fill Constructor
      // REQUIRES: Capacity > 0
      // MODIFIES: *this
      // EFFECTS: Creates a vector with size num_elements, all assigned to val
      vector ( size_t num_elements, const T& val ) : _size( num_elements ), _capacity( num_elements ), _elts(new T[ num_elements ]())
         {
         for (T *ptr = _elts, * const end = _elts + _size; _elts != end; )
              *ptr++ = val;
         }

      // Copy Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Creates a clone of the vector other
      vector ( const vector<T>& other ) : vector ( other.num_elements ) 
         {
            for (T *ptr = _elts, *otherPtr = other._elts, * const end = _elts + _size; _elts != end; )
               *ptr++ = otherPtr++;
         }

      // Assignment operator
      // REQUIRES: Nothing
      // MODIFIES: *this
      // EFFECTS: Duplicates the state of other to *this
      vector &operator= ( const vector<T>& other )
         {
         // Doesn't check %other != *this since it's the programmer's responsiblity to make sure such isn't the case
         vector<T> temp( other ); // utilize copy constructor
         swap( temp );
         return *this;
         }

      // Move Constructor
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves other in a default constructed state
      // EFFECTS: Takes the data from other into a newly constructed vector
      vector ( vector<T>&& other ) : _size( std::move( other._size ) ), _capacity( std::move( other._capacity ) ), _elts( std::move( other._elts ) )
         {
         other._elts = nullptr; // To ensure no double deletes
         swap( other, vector<T>() );
         }

      // Move Assignment Operator
      // REQUIRES: Nothing
      // MODIFIES: *this, leaves otherin a default constructed state
      // EFFECTS: Takes the data from other in constant time
      vector& operator= ( vector<T>&& other )
         {
         delete[] _elts;
         _elts = other._elts;
         other._elts = nullptr;
         _size = other._size;
         _capacity = other.capacity;
         swap( other, vector<T>() );
         return *this;
         }

      // REQUIRES: new_capacity > capacity()
      // MODIFIES: capacity()
      // EFFECTS: Ensures that the vector can contain size() = new_capacity
      //    elements before having to reallocate
      void reserve ( size_t newCapacity )
         {
            if (newCapacity <= _capacity) return;

            T* newElements = new T[newCapacity];

            // 
            for (size_t i = 0; i < _size; ++i) 
               newElements[i] = _elts[i];

            delete [] _elts;
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
            return _elts[i];
         }

      // REQUIRES: 0 <= i < size()
      // MODIFIES: Nothing
      // EFFECTS: Get a const reference to the ith element
      const T& operator[ ] ( size_t i ) const
         {
            assert (i >= 0 && i < _size);
            return _elts[i];
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size(), capacity()
      // EFFECTS: Appends the element x to the vector, allocating
      //    additional space if neccesary
      void pushBack ( const T& x )
         {
            if (_capacity == 0)
               reserve(BUCKET_SIZE);
            else if (_capacity == _size) 
               reserve(2 * _capacity);
            
            _elts[_size++] = x;
         }

      // REQUIRES: Nothing
      // MODIFIES: this, size()
      // EFFECTS: Removes the last element of the vector,
      //    leaving capacity unchanged
      void popBack ( )
         {
            if (_size == 0) return;
            _elts[--_size].~T();
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to the 
      //    first element of the vector
      T* begin ( )
         {
            return &_elts[0];
         }

      // REQUIRES: Nothing
      // MODIFIES: Allows mutable access to the vector's contents
      // EFFECTS: Returns a mutable random access iterator to 
      //    one past the last valid element of the vector
      T* end ( )
         {
            return &_elts[_size];
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

   private:

      T* _elts;
      size_t _size;
      size_t _capacity;



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
   };
   
#endif