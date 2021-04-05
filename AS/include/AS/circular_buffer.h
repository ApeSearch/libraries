
#pragma once

#ifndef CIRCULAR_BUFFER_H_APESEARCH
#define CIRCULAR_BUFFER_H_APESEARCH

#include <initializer_list> // for std::initializer_list
#include <iostream> // for std::ostream
#include <assert.h> // for assert()
#include <utility> // using std::forward<T>

//! Inferface for circular_buffer below

namespace APESEARCH
{
//------------------------------------------------------------------------------------------------
//
//                                  Buffer Declaration
//
//------------------------------------------------------------------------------------------------
/*
 * Purely abstract class
 * The rationale for creating an interface for the buffer used in circular_buffer 
 * is to allow for both dynamic and local memory to be utilized; that is if the programmer
 * knows what the size will be (determined by experimentation) it is better to just declare a
 * static size (T arr[KNOWNSIZE]) to avoid dynamic allocation.
 * 
 * However, this allows for an implementation for dynamic memory (Just it has to be overrided)
 * 
 * An example buffer (defaultBuffer is declared and defined in circular_buffer.inl)
*/
// Purely abstract clas
// Use this as the interface when deciding to use a buffer
template<typename T>
struct Buffer
{
    typedef T value_type;

    virtual ~Buffer() {}
    inline virtual value_type *getBuffer() noexcept = 0;
    inline virtual void insert(const T&, size_t) noexcept = 0;
    inline virtual void insert( T&&, size_t ) noexcept;
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
template<typename T, size_t _Sizeu>
struct defaultBuffer;
}


//------------------------------------------------------------------------------------------------
//
//                                  circular_buffer Declaration
//
//------------------------------------------------------------------------------------------------
/*
 * A buffer which takes advantage of a constant capacity in order to maximize spacial locality.
 * //!Not thread safe however (needs to be augmented for such)
*/
// The current “tail” position (incremented when elements are added)
//The current “head” (incremented when elements are removed)
template <class T, class buffer_type = DEFAULT::defaultBuffer<T,20u> >
class circular_buffer
{

public:
    typedef T                     value_type;
    typedef value_type&           reference;
    typedef const value_type&     const_reference;
    typedef std::size_t           size_type;

    circular_buffer() noexcept {}
    circular_buffer( size_t ) noexcept;
    circular_buffer( buffer_type && ) noexcept;
    circular_buffer( std::initializer_list<value_type> );
    ~circular_buffer();

    void reset();

    void putOverwrite( const value_type & ) noexcept;
    inline bool put( const value_type & ) noexcept;
    inline bool emplace( value_type&& ) noexcept;
    void push_back( const value_type & val) noexcept { put( std::forward<const value_type&>( val ) ); }
    void emplace_back( value_type && val ) noexcept { emplace( std::forward< value_type >( val ) ); }

    reference front() noexcept;
    const_reference front() const noexcept { return std::move( front() ); }
    const_reference& back() const noexcept;
    value_type get();
    void pop_front() { get(); }

    inline bool empty() const { return !_full && head == tail; }
    inline bool full() const { return _full; } // return ( head + 1 ) % capacity == tail;
    inline size_t capacity() const { return buffer.getCapacity(); }
    size_t size() const;
    size_t getHead() const { return head; }
    size_t getTail() const { return tail; }

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

    void interal_push();
    void internal_pop();
};

} // end namespace APESEARCH

#include "circular_buffer.inl" // circular_buffer Inline Implementation ( defaultBuffer and circular_buffer )

#endif // CIRCULAR_BUFFER_H_APESEARCH