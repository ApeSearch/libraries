
#pragma once

#ifndef PTHREAD_POOL_H_APESEARCH
#define PTHREAD_POOL_H_APESEARCH

#include <iostream>

#ifdef testing
    #include<memory>
    using std::shared_ptr;
    using std::make_shared;
#else
    #include "shared_ptr.h"
    using APESEARCH::shared_ptr;
    using APESEARCH::make_shared;
#endif
#include <exception> //for std::current_exception
#include "queue.h" 
#include "vector.h"
#include "atomic_queue.h" // for APESEARCH::atomic_queue
#include "mutex.h"
#include "condition_variable.h"
#include <functional> // fod std::bind
#include <future> // for std::future, get_future()
#include <pthread.h> // for pthread
#include <utility> // for std::forward
#if __cplusplus >= 201703L
   #include <optional>
   using std::optional;
#else
   #include <experimental/optional>
   using std::experimental::optional;
#endif
#include <atomic> // for std::atomic_bool
#include <exception>
//#include <semaphore> // for std::counting_semaphore

#define DEFAULTMAXSUBMITS 100

namespace APESEARCH
{

struct defer_init_t { explicit defer_init_t() = default; };
#if __cplusplus == 201703L
    inline constexpr defer_init_t  defer_init{};
#else
    extern const defer_init_t  defer_init;
#endif

using Func = std::function<void()>;

template<class Container = deque<Func> >
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
        std::function<void()> func;
        while ( true ) // runs until party ends
           {
            {
            APESEARCH::unique_lock<APESEARCH::mutex> uniqLock( pool->prodMutex );
            auto pred = [this]() -> bool { return pool->halt.load() || !pool->_queue.empty(); };
            pool->waitingProd.wait( uniqLock, pred );
            if ( pool->halt.load() && pool->_queue.empty() )
               return;

            func = pool->_queue.dequeue().value();
            pool->waitingCons.notify_one();
            } // end ~uniqLock()
            func();
           } // end while 
        } // end operator()()
        
   }; // end ThreadWorker
   APESEARCH::atomic_queue< std::function<void()>, Container > _queue;
   APESEARCH::vector< pthread_t > _threads;
   const size_t maxSubmits;
   APESEARCH::mutex prodMutex;
   APESEARCH::mutex consumerMutex;
   APESEARCH::condition_variable waitingProd;
   APESEARCH::condition_variable waitingCons;
   std::atomic<bool> halt;

   static void *indirectionStrikesAgain( void *func )
       {
        ThreadWorker *funcPtr = reinterpret_cast<ThreadWorker *>( func );
        try 
	     {
           (*funcPtr)();
           delete funcPtr;
        } // end try
        catch (...) // Ensures no memory leak
           {
            delete funcPtr;
            throw std::current_exception();
           } // end catch
        return nullptr;
       } // end indirectionStrikesAgain()

public:
   PThreadPool( size_t numThreads, defer_init_t, size_t _maxSubmits = DEFAULTMAXSUBMITS ) 
         noexcept : _threads( numThreads ), maxSubmits( _maxSubmits ), halt( true ) {}

   PThreadPool( size_t numThreads, size_t _maxSubmits = DEFAULTMAXSUBMITS ) noexcept 
         : PThreadPool( numThreads, defer_init, _maxSubmits )
      {
       init();
      } // end PThreadPool()

   ~PThreadPool() { shutdown(); }

    PThreadPool( const PThreadPool & ) = delete;
    PThreadPool( PThreadPool && ) = delete;

    PThreadPool& operator=( const PThreadPool& ) = delete;
    PThreadPool& operator=( const PThreadPool&& ) = delete;

   // Gives user program more freedom with when they want to allow threads to start running
   void init()
      {
       // Ensures that can only run if thread pool has halted
       if ( !halt.load() )
          return;
       halt.store( false ); // Gives okay for threads to run

       unsigned n = 0;
       for( auto itr = _threads.begin(); itr != _threads.end(); ++itr, ++n )
          {
           ThreadWorker *startingFunc = new ThreadWorker( this, n );
           if ( pthread_create( &*itr, NULL, indirectionStrikesAgain, static_cast<void *>( startingFunc ) ) != 0 )
               {
               delete startingFunc;
               // Any threads that already started are signaled to free their resources
               halt.store( true ); 
               waitingProd.notify_all();
               throw std::runtime_error("Pthread_create returned an error");
               } // end if
          } // end for
      } // end init()

   // Signals threads to join and free any held resources
   void shutdown()
      {
      // Ensure that value is only changed once
      bool expected = false;
      if ( !halt.compare_exchange_weak( expected, true,  std::memory_order_release ) ) { return; }
      // Make sure every thread is woken up
      waitingProd.notify_all();

      for( auto itr = _threads.begin(); itr != _threads.end(); ++itr )
          {
           if ( pthread_join( *itr, nullptr ) != 0 )
                throw std::runtime_error("Pthread_join returned an error");
          } // end for

      } // end shutdown()

   // Tasks are submitted (as functions ) and a future object is returned to allow calling thread
   // to obtain the results when the function finishes
   template<typename Func, typename...Args>
   auto submit( Func&& f, Args&&... args ) -> std::future<decltype( f(args...) )>
      {
      APESEARCH::unique_lock<APESEARCH::mutex> uniqLock( consumerMutex );
      waitingCons.wait( uniqLock, [this]() { return halt.load() || _queue.size() < maxSubmits; } );
      // Don't allow thread to continue submitting
      if ( halt.load() )
         return std::future<decltype( f(args...) )>();

      // Create a function object
      std::function<decltype( f(args...) )( )> func = std::bind( std::forward<Func>( f ), std::forward<Args>( args )... );
      auto taskPtr = make_shared<std::packaged_task<decltype( f(args...) )( )>>( func );
      
      // Wrap taskPtr inside a void function
      std::function<void()> wrapperFunc = [taskPtr]() { 
         (*taskPtr)(); // invoke the callable entity here
      };

      _queue.enqueue( wrapperFunc );

      // Wake up one thread
      waitingProd.notify_one();

      return taskPtr->get_future();
      } // end submit()
};

} // end namespace APESEARCH

#endif
