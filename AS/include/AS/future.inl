
//------------------------------------------------------------------------------------------------
//
//                                 shared_state_value Inline Definition 
//
//------------------------------------------------------------------------------------------------
//*** Look at future.h For full interface

#include "algorithms.h"
#include "string.h"

// Currently inside APESEARCH namespace

namespace APESEARCH
{

template<class T>
template<class Arg>
void shared_state<T>::set_value( Arg&& arg )
   {
    unique_lock<mutex> lk( stateMut );
    if ( has_value() )
       throw future_error( make_error_code(future_errc::promise_already_satisfied ));
    ::new( &value ) T( std::forward<Arg>( arg ) );
    state |= base_shared_state::constructed | base_shared_state::ready;
    cv.notify_one();
   } // end shared_state_value()

template<class T>
T shared_state<T>::move()
   {
    unique_lock<mutex> lk( stateMut );
    get_value_helper( lk );
    return std::move( *reinterpret_cast<T*>( &value ) );
   } // end move()

template<class T>
typename std::add_lvalue_reference<T>::type shared_state<T>::copy()
   {
    unique_lock<mutex> lk( stateMut );
    get_value_helper( lk );
    return *reinterpret_cast<T*>( &value );
   } // end shared_state_value<T>::copy()

//------------------------------------------------------------------------------------------------
//
//                                 shared_state_ref Inline Definition 
//
//------------------------------------------------------------------------------------------------

template<class T>
void shared_state<T&>::set_value( T& arg )
   {
    unique_lock<mutex> lk( stateMut );
    value = &arg;
    state |= base_shared_state::constructed | base_shared_state::ready;
    cv.notify_one();
   } // end set_value()

template<class T>
T& shared_state<T&>::copy( T& arg )
   {
    unique_lock<mutex> lk( stateMut );
    get_value_helper( lk );
    return *value;
   }

//------------------------------------------------------------------------------------------------
//
//                                 Promise Inline Definition 
//
//------------------------------------------------------------------------------------------------

template<class R>
promise<R>::promise() : _state( new shared_state<R>() ) {}

template<class R>
promise<R>::promise( promise<R>&& rhs ) noexcept : _state( nullptr )
   {
   swap( _state, rhs._state );   
   } // end promise()

template<class R>
promise<R>::~promise()
   {
   if ( _state )
      {
      if ( !_state->has_value() )
         _state->set_exception( std::make_exception_ptr( 
            future_error( make_error_code ( future_err::broken_promise)  ) ) );
      if ( !_state->invalidate_promise_if_future_attached() )
         delete _state;
      // Else future is still considered alive and _state shouldn't be deleted
      } // end if
   } // end ~promise()

template<class R>
promise<R>& promise<R>::operator=( promise<R>&& rhs ) noexcept
   {
   promise<R> temp;
   }


struct future_error_category : public std::system_category
   {
   const char * what() const throw()
      {
      return "future error";
      }
   string message( const int errorVal ) cosnt
      {
      switch( static_cast<future_errc>( errorVal ) )
         {
         case future_errc::broken_promise:
            return string("The associated promise has been destructed prior "
                      "to the associated state becoming ready.");
         case future_errc::future_already_retrieved:
            return string("The future has already been retrieved from "
                      "the promise or packaged_task.");
         case future_errc::promise_already_satisfied:
            return string("The state of the promise has already been set.");
         case future_errc::no_state:
            return string("Operation not permitted on an object without "
                      "an associated state.");
         default:
            return string("Unexcpeted value");
         } // end switch
      
      } // end message()
   }; // end future_error_category;


const std::error_category& future_category() noexcept
   {
   static future_error_category fut;
   return fut;
   }


} // end APESEARCH 