
#pragma once

#ifndef PTHREAD_POOL_H_APESEARCH
#define PTHREAD_POOL_H_APESEARCH

#include <iostream>

#ifdef testing
    #include<queue>
    using std::queue;
    #include<vector>
    using std::vector;
    #include<memory>
    using std::shared_ptr;
    using std::make_shared;
#else
    #include "queue.h"
    using APESEARCH::queue;
    #include "vector.h"
    using APESEARCH::vector;
#endif

#include <functional> // fod std::bind
#include <future> // for std::future, get_future()
//#include "mutex.h" // for APESEARCH::mutex
#include "atomic_queue.h" // for APESEARCH::atomic_queue
#include <pthread.h> // for pthread
//#include <semaphore> // for std::counting_semaphore
#include <mutex>
#include <condition_variable> //! Need to create own
#include <utility> // for std::forward
#include <optional> // for std::optional
#include <atomic> // for std::atomic_bool
#include <exception>

namespace APESEARCH
{

struct defer_init_t { explicit defer_init_t() = default; };
#if __cplusplus == 201703L
    inline constexpr defer_init_t  defer_init{};
#else
    extern const defer_init_t  defer_init;
#endif

class PThreadPool
{
   class ThreadWorker
   {
      PThreadPool *pool;
      unsigned thrd_id;

    public:
      ThreadWorker( PThreadPool *_pool, const unsigned _thrd_id ) :
         pool( _pool ), thrd_id( _thrd_id ) {}
    
    // Used by threads 
    void operator()()
        {
        std::optional<std::function<void()>> func;
        while ( true ) // runs until party ends
           {
            //pool->consumerSema.acquire();
            {
            std::unique_lock<std::mutex> uniqLock( pool->cvMutex );
            auto pred = [this]() -> bool { return pool->halt.load() || !pool->_queue.empty(); };
            pool->cv.wait( uniqLock, pred );
            if ( pool->halt.load() && pool->_queue.empty() )
               return;

            func = pool->_queue.dequeue();
            if ( func )
               func.value()();
            } // ~uniqLock
           } // end while 
        }
        
   }; // end ThreadWorker
   atomic_queue< std::function<void()> > _queue;
   vector< pthread_t > _threads;
   size_t _numThreads;
   //std::counting_semaphore consumerSema; => Requires c++20
   std::mutex cvMutex;
   std::condition_variable cv;
   std::atomic<bool> halt = false;

   static void *indirectionStrikesAgain( void *func )
       {
        ThreadWorker *funcPtr = reinterpret_cast<ThreadWorker *>( func );
        (*funcPtr)();

        delete funcPtr;
        return nullptr;
       } // end indirectionStrikesAgain()

public:
   PThreadPool( size_t numThreads, defer_init_t ) noexcept : _threads( numThreads), _numThreads( numThreads )
      {
      }
   PThreadPool( size_t numThreads ) noexcept : PThreadPool( numThreads, defer_init )
      {
       init();
      } // end PThreadPool()

   ~PThreadPool()
      {
      shutdown();
      }

    PThreadPool( const PThreadPool & ) = delete;
    PThreadPool( PThreadPool && ) = delete;

    PThreadPool& operator=( const PThreadPool& ) = delete;
    PThreadPool& operator=( const PThreadPool&& ) = delete;

   void init()
      {
       unsigned n = 0;
       for( auto itr = _threads.begin(); itr != _threads.end(); ++itr, ++n )
          {
           ThreadWorker *startingFunc = new ThreadWorker( this, n );
           if ( pthread_create( &*itr, NULL, indirectionStrikesAgain, static_cast<void *>( startingFunc ) ) != 0 )
                throw;
          } // end for
      }
   void shutdown()
      {
      // Ensure that value is only changed once
      bool expected = false;
      if ( !halt.compare_exchange_weak( expected, true,  std::memory_order_release ) ) { return; }
      // Make sure every thread is woken up
      //for( unsigned n = 0; n < _numThreads; ++n )
      //   consumerSema.release();
      cv.notify_all();

      for( auto itr = _threads.begin(); itr != _threads.end(); ++itr )
          {
           if ( pthread_join( *itr, nullptr ) != 0 )
                throw;
          } // end for

      } // end shutdown()

   template<typename Func, typename...Args>
   auto submit( Func&& f, Args&&... args ) -> std::future<decltype( f(args...) )>
      {
      // Create a function object
      std::function<decltype( f(args...) )( )> func = std::bind( std::forward<Func>( f ), std::forward<Args>( args )... );
      auto taskPtr = make_shared<std::packaged_task<decltype( f(args...) )( )>>( func );
      
      // Wrap taskPtr inside a void function
      std::function<void()> wrapperFunc = [taskPtr]() { 
         (*taskPtr)(); // invoke the callable entity here
      };

      _queue.enqueue( wrapperFunc );

      // Wake up one thread
      //consumerSema.release();
      cv.notify_one();

      return taskPtr->get_future();
      } // end submit()
};

} // end namespace APESEARCH

#endif