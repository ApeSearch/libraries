
#pragma once

#ifndef ATOMIC_QUEUE_H_APESEARCH
#define ATOMIC_QUEUE_H_APESEARCH

#include "queue.h"
#include "circular_buffer.h"


// A thread-safe queue
#include "mutex.h"
#include <utility>
#include "queue.h"
#include <assert.h>
#if __cplusplus >= 201703L
   #include <optional>
   using std::optional;
#else
   #include <experimental/optional>
   using std::experimental::optional;
#endif

namespace APESEARCH
{

template<typename T, class Container = std::deque<T> >
class atomic_queue
{
   queue<T, Container> safeQueue;
   mutex queueMutex;
   static mutex classMutex; // dealing with move constructors
public:
   atomic_queue() {}
   atomic_queue( Container&& c ) : safeQueue( std::forward<Container>( c ) )
      {
      }

   atomic_queue( atomic_queue& other )
      {
       unique_lock<mutex> uniqLock( classMutex );
       unique_lock<mutex> ownLock( queueMutex );
       unique_lock<mutex> otherLock( other.queueMutex );
       safeQueue = other.safeQueue;
      }

   void swap( atomic_queue& other ) noexcept
      {
       unique_lock<mutex> uniqLock( classMutex );
       unique_lock<mutex> ownLock( queueMutex );
       unique_lock<mutex> otherLock( other.queueMutex );
       safeQueue.swap( other.safeQueue );
      }

   inline bool empty() 
      {
        unique_lock<mutex> uniqLock( queueMutex );
        return safeQueue.empty();
      }
   inline size_t size()
      {
        unique_lock<mutex> uniqLock( queueMutex );
        return safeQueue.size();
      }
   void enqueue( T& val )
      {
         unique_lock<mutex> uniqLock( queueMutex );
         safeQueue.push( val );
      }
   template <class... Args> 
#ifdef __cplusplus >= 201703L
   #define RETURNTYPE decltype(auto)
#else
   #define RETURNTYPE void
#endif
   RETURNTYPE emplace(Args&&... args) 
      {
      unique_lock<mutex> uniqLock( queueMutex );
      safeQueue.emplace( std::forward<Args>(args)... );
      }
   
   optional<T> dequeue()
      {
      unique_lock<mutex> uniqLock( queueMutex );

      if( safeQueue.empty() )
         {
         assert( false ); // for testing
         return {};
         }
      T val = std::move( safeQueue.front() );
      safeQueue.pop();
      return val;
      } // end dequeue()
};

} //end namespace

#endif