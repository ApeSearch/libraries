
#pragma once

#ifndef ATOMIC_QUEUE_H_APESEARCH
#define ATOMIC_QUEUE_H_APESEARCH

#ifdef testing
    #include<queue>
    using std::queue;
#else
    #include "queue.h"
    using APESEARCH::queue;
#endif



// A thread-safe queue
#include "mutex.h"
#include <utility>
#include <queue>
#if __cplusplus >= 201703L
   #include <optional>
   using std::optional;
#else
   #include <experimental/optional>
   using std::experimental::optional;
#endif

namespace APESEARCH
{

template<typename T>
class atomic_queue
{
   queue<T> safeQueue;
   mutex queueMutex;
   static mutex classMutex; // dealing with move constructors
public:
   atomic_queue() {}
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
          return {};
       T val = std::move( safeQueue.front() );
       safeQueue.pop();
       return std::move( val );
       } // end dequeue()
};

} //end namespace

#endif