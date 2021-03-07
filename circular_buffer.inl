
//------------------------------------------------------------------------------------------------
//
//                                  cirular_buffer Inline Definition 
//
//------------------------------------------------------------------------------------------------
#include <cstddef> // for size_t
#include "algorithms.h"
#include "assert.h"


template <class T, class buffer_type>
APESEARCH::circular_buffer<T, buffer_type>::circular_buffer( size_t _capacity ) noexcept : buffer( _capacity )
   {
   }

template <class T, class buffer_type>
APESEARCH::circular_buffer<T, buffer_type>::circular_buffer( buffer_type&& _buffer ) noexcept : buffer( _buffer )
   {
   }

template <class T, class buffer_type>
APESEARCH::circular_buffer<T, buffer_type>::circular_buffer( std::initializer_list<T> list ) : circular_buffer( list.size() )
   {
    APESEARCH::for_each( list.begin(), list.end(), [this](const T& val) { put( val ); } );
   }

template <class T, class buffer_type>
APESEARCH::circular_buffer<T, buffer_type>::~circular_buffer()
   {
   }

template <class T, class buffer_type>
void APESEARCH::circular_buffer<T, buffer_type>::reset()
   {
    head = tail = _full = 0;
   }

template <class T, class buffer_type>
void APESEARCH::circular_buffer<T, buffer_type>::putOverwrite( const T& val ) noexcept
   {
    buffer.insert( val, tail );
    advance_ptr();
   }

template <class T, class buffer_type>
bool APESEARCH::circular_buffer<T, buffer_type>::put( const T& val ) noexcept
   {
    if ( _full )
       return false;

    buffer.insert( val, tail );
    advance_ptr();
    return true;
   }

template <class T, class buffer_type>
const T& APESEARCH::circular_buffer<T, buffer_type>::front() const noexcept
   {
    assert( !empty() );
    return buffer.get( head );
   }

template <class T, class buffer_type>
T APESEARCH::circular_buffer<T, buffer_type>::get()
   {
    assert( !empty() );
    size_t currHead = head;
    retreat_ptr();
    return buffer.get( currHead );
   }

template <class T, class buffer_type>
size_t APESEARCH::circular_buffer<T, buffer_type>::size() const 
   {
    assert( buffer.getCapacity() >= head && buffer.getCapacity() >= tail );
    if(!_full)
        return tail >= head ? tail - head : buffer.getCapacity() - head + tail;
    return buffer.getCapacity();
   }

template <class T, class buffer_type>
void APESEARCH::circular_buffer<T, buffer_type>::advance_ptr()
   {
    
    if( _full )
       {
        if( ++head == buffer.getCapacity() )
            head = 0;
       }
       
    if ( ++tail == buffer.getCapacity() )
        tail = 0;
    _full = tail == head;
   }

template <class T, class buffer_type>
void APESEARCH::circular_buffer<T, buffer_type>::retreat_ptr()
   {
    _full = false;
    if ( ++head == buffer.getCapacity() )
        head = 0;
   } // end retreat_ptr()

/*
template<typename T, class buffer_type>
std::ostream& APESEARCH::operator<<(std::ostream& os, const APESEARCH::circular_buffer<T, buffer_type>& cbuf)
   {
    cbuf.buffer.print( os, cbuf.head, cbuf.size() );
    return os;
   }
*/

