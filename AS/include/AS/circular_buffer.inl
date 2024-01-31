
//------------------------------------------------------------------------------------------------
//
//                                  cirular_buffer Inline Definition 
//
//------------------------------------------------------------------------------------------------
#include <cstddef> // for size_t
#include "algorithms.h" // for APESEARCH::for_each()
#include <exception> // for std::run_time


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
    for_each( list.begin(), list.end(), [this](const T& val) { put( val ); } );
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
    interal_push();
   }

template <class T, class buffer_type>
bool APESEARCH::circular_buffer<T, buffer_type>::put( const T& val ) noexcept
   {
    if ( _full )
       return false;

    buffer.insert( val, tail );
    interal_push();
    return true;
   }

/*
template <class T, class buffer_type>
bool APESEARCH::circular_buffer<T, buffer_type>::emplace( T&& val ) noexcept
   {
    if ( _full )
       return false;

    buffer.insert( std::forward< T&& >( val ), tail );
    interal_push();
    return true;
   }
*/

template <class T, class buffer_type>
T& APESEARCH::circular_buffer<T, buffer_type>::front() noexcept
   {
    assert( !empty() );
    return buffer.get( head );
   }

template <class T, class buffer_type>
const T& APESEARCH::circular_buffer<T, buffer_type>::back() const noexcept 
   {
   assert( !empty() );
   return buffer.get( tail );
   }

template <class T, class buffer_type>
T APESEARCH::circular_buffer<T, buffer_type>::get()
   {
    assert( !empty() );
    size_t currHead = head;
    internal_pop();
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
void APESEARCH::circular_buffer<T, buffer_type>::interal_push()
   {
    
    if( _full ) // Overwrite value
       {
       assert( head == tail );
       if( ++tail == buffer.getCapacity() )
          head = tail = 0;
       else
          ++head;
       }
    else
       {
       if ( ++tail == buffer.getCapacity() )
          tail = 0;
       _full = tail == head;
       }
   } // end internal_push()

template <class T, class buffer_type>
void APESEARCH::circular_buffer<T, buffer_type>::internal_pop()
   {
    _full = false;
    if ( ++head == buffer.getCapacity() )
        head = 0;
   } // end internal_pop()
   

//------------------------------------------------------------------------------------------------
//
//                                  defaultBuffer Inline Definition 
//
//------------------------------------------------------------------------------------------------

namespace APESEARCH
{
namespace DEFAULT
{
// An example
template<typename T, size_t _Sizeu = 20u>
struct defaultBuffer : public Buffer<T>
{
   static const size_t capacity = _Sizeu;
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
       assert( index < defaultBuffer::capacity );
       buf[ index ] = val;
      }
   
   inline void insert( size_t index, T&& val ) noexcept
      {
       assert( index < defaultBuffer::capacity );
       buf[ index ] = std::forward<T>( val );
      }
   template< typename ...Args >
   inline void insert(size_t index, Args&& ...val ) noexcept
      {
       assert( index < defaultBuffer::capacity );
       buf[ index ] = T ( std::forward<Args>( val )... );
      }

   inline virtual T& get(size_t index)
      {
       assert( index < defaultBuffer::capacity );
       return buf[ index ];
      }
   inline size_t getCapacity() const
      {
       return defaultBuffer::capacity;
      }
   inline value_type *begin() noexcept
      {
      return buf;
      }
   inline static ssize_t max() 
      {
      return defaultBuffer::capacity;
      }
      //! Not every time can be printed
    void print( std::ostream& os, const size_t , const size_t sizeOf ) const
       {
        assert( sizeOf <= defaultBuffer::capacity );
        os << "[ ";
        for (size_t n = 0; n < sizeOf; ++n )
           {
            //os << buf[ ( head + n ) % defaultBuffer::capacity ] << ' ';
           }
        os << "]";
       }
private:
   T buf[ defaultBuffer::capacity ];
};
} // end namespace DEFAULT
} // end namespace APESEARCH
