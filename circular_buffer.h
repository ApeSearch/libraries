
#pragma once

#ifndef CIRCULAR_BUFFER_H_APESEARCH
#define CIRCULAR_BUFFER_H_APESEARCH

#ifdef testing
    #include <vector>
    using std::vector;
#else
    #include "vector.h"
    using APESEARCH::vector;
#endif

#include <initializer_list>
#include <utility>
#include <exception>
#include <iostream>
#include <assert.h>

namespace APESEARCH
{

// Purely abstract clas
// Use this when using a particular buffer
template<typename T>
struct Buffer
{
    typedef T value_type;

    virtual ~Buffer() {}
    inline virtual value_type *getBuffer() noexcept = 0;
    inline virtual void insert(const T&, size_t) noexcept = 0;
    inline virtual T& get(size_t) = 0;
    inline virtual size_t getCapacity() const = 0;
    inline virtual T *begin() noexcept = 0;
    virtual void print( std::ostream& os, const size_t head, const size_t size ) const = 0;

    //! Required for circular_buffer
    // virtual Buffer() = 0;
    // virtual Buffer( const Buffer&& buf ) noexcept = 0;
    // virtual Buffer( const size_t ) = 0;
};

namespace DEFAULT
{
#define CAPACITY 20
static const size_t capacity = CAPACITY;
// An example
template<typename T>
struct defaultBuffer : public Buffer<T>
{
   typedef T value_type;
   defaultBuffer() {}
   defaultBuffer( const defaultBuffer& )
      {
       // Does nothing in this case since there's no 
      }
   defaultBuffer( const size_t numOfElem )
      {
       if ( static_cast<size_t>( max() ) < numOfElem )
          {
          throw std::runtime_error("Attempted to create a buffer larger than max");
          }
      }
   inline value_type *getBuffer() noexcept
      {
       return buf;
      }
   inline void insert(const T& val, size_t index) noexcept
      {
       assert( index < CAPACITY );
       buf[ index ] = val;
      }
   inline virtual T& get(size_t index)
      {
       assert( index < CAPACITY );
       return buf[ index ];
      }
   inline size_t getCapacity() const
      {
       return CAPACITY;
      }
   inline value_type *begin() noexcept
      {
      return buf;
      }
   inline static ssize_t max() 
      {
      return CAPACITY;
      }
    void print( std::ostream& os, const size_t head, const size_t sizeOf ) const
       {
        assert( sizeOf <= CAPACITY );
        os << "[ ";
        for (size_t n = 0; n < sizeOf; ++n )
           {
            os << buf[ ( head + n ) % CAPACITY ] << ' ';
           }
        os << "]";
       }
private:
   T buf[ CAPACITY ];
};
} // end namespace DEFAULT

// The current “tail” position (incremented when elements are added)
//The current “head” (incremented when elements are removed)
template <class T, class buffer_type = DEFAULT::defaultBuffer<T> >
class circular_buffer
{

public:
    typedef T value_type;

    circular_buffer() noexcept {}
    circular_buffer( size_t ) noexcept;
    circular_buffer( buffer_type && ) noexcept;
    circular_buffer( std::initializer_list<value_type> );
    ~circular_buffer();

    void reset();

    void putOverwrite( const value_type & ) noexcept;
    bool put( const value_type & ) noexcept;

    const value_type& front() const noexcept;
    value_type get();

    inline bool empty() const { return !_full && head == tail; }
    inline bool full() const { return _full; } // return ( head + 1 ) % capacity == tail;
    inline size_t capacity() const { return buffer.getCapacity(); }
    size_t size() const;

    friend std::ostream& operator<<(std::ostream& os, const circular_buffer& cbuf)
       {
        cbuf.buffer.print( os, cbuf.head, cbuf.size() );
        return os;
       }

private:
    buffer_type buffer; // Provides an abstraction of the buffer
    size_t head = 0;
    size_t tail = 0;
    bool _full = false;

    void advance_ptr();
    void retreat_ptr();
};

} // end namespace APESEARCH

#include "circular_buffer.inl"




#endif // CIRCULAR_BUFFER_H_APESEARCH