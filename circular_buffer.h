
#pragma once

#ifndef CIRCULAR_BUFFER_H_APESEARCH
#define CIRCULAR_BUFFER_H_APESEARCH

#include <initializer_list> // for std::initializer_list
#include <iostream> // for std::ostream
#include <assert.h> // for assert()

namespace APESEARCH
{
//------------------------------------------------------------------------------------------------
//
//                                  Buffer Declaration
//
//------------------------------------------------------------------------------------------------

// Purely abstract clas
// Use this as the interface when deciding to use a buffer
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

    //! Required for circular_buffer ( however cannot be virtual due to the nature of c++ )
    // virtual Buffer() = 0;
    // virtual Buffer( const Buffer&& buf ) noexcept = 0;
    // virtual Buffer( const size_t ) = 0;
};

// An example of an implementation of Buffer
namespace DEFAULT{
template<typename T>
struct defaultBuffer;
}


//------------------------------------------------------------------------------------------------
//
//                                  circular_buffer Declaration
//
//------------------------------------------------------------------------------------------------

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

#include "circular_buffer.inl" // circular_buffer Inline Implementation ( defaultBuffer and circular_buffer )

#endif // CIRCULAR_BUFFER_H_APESEARCH