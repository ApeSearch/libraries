//------------------------------------------------------------------------------------------------
//
//                                  unique_lock Inline Definition 
//
//------------------------------------------------------------------------------------------------
//*** Look at mutex.h For full interface

#include "algorithms.h" // for APESEARCH::swap()

   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::unique_lock() noexcept : mutexOf( nullptr ), hasLock( false ) {}


   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::unique_lock(Mutex& m) : mutexOf( &m )
      {
      lock();
      }
   // does not acquire ownership of the mutex
   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::unique_lock(Mutex& m, defer_lock_t) noexcept : mutexOf( &m ) 
      {
      }

   // tries to acquire ownership of the mutex without blocking
   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::unique_lock(Mutex& m, try_to_lock_t) : mutexOf( &m ), hasLock( m.try_lock() )
      {
      }

   // assume the calling thread already has ownership of the mutex
   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::unique_lock(Mutex& m, adopt_lock_t) : mutexOf( &m ), hasLock( true )
      {
      }

   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::~unique_lock()
      {
       if ( mutexOf && hasLock )
          mutexOf->unlock();
      }


   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::unique_lock( unique_lock<Mutex>&& u ) noexcept : mutexOf( nullptr )
      {
       // Assumes that the lock is already acquired
       u.swap( *this );
      }

   template<class Mutex>
   APESEARCH::unique_lock<Mutex>& APESEARCH::unique_lock<Mutex>::operator=( unique_lock<Mutex>&& u ) noexcept
      {
      unique_lock<Mutex> temp;
      swap( u, temp );
      temp.swap( *this ); // swap what was taken from temp (allows temp to do whatever freeing is necessary)
      return *this;
      }

   template<class Mutex>
   inline void APESEARCH::unique_lock<Mutex>::lock()
      {
      assert( mutexOf && !hasLock );
      mutexOf->lock();
      hasLock = true;
      }

   template<class Mutex>
   inline bool APESEARCH::unique_lock<Mutex>::try_lock()
      {
      if ( owns_lock() )
         throw std::system_error( EDEADLK, std::generic_category(), "Attempting to try_lock when lock is already owned");
      return hasLock = mutexOf->try_lock();
      }

   template<class Mutex>
   void APESEARCH::unique_lock<Mutex>::unlock()
      {
      assert( owns_lock() );
      mutexOf->unlock();
      hasLock = false;
      }

   template<class Mutex>
   void APESEARCH::unique_lock<Mutex>::swap( unique_lock<Mutex>& u ) noexcept
     {
      swap( mutexOf, u.mutexOf );
      swap( hasLock, u.hasLock );
     } // end swap()

   template<class Mutex>
   Mutex* APESEARCH::unique_lock<Mutex>::release() noexcept
      {
      mutex_type *ptr = nullptr;
      swap( ptr, mutexOf );
      return ptr;
      } // end release()

   template<class Mutex>
   inline bool APESEARCH::unique_lock<Mutex>::owns_lock() const noexcept
      {
      return unique_lock<Mutex>::operator bool () ;
      } // end owns_lock()

   template<class Mutex>
   APESEARCH::unique_lock<Mutex>::operator bool () const noexcept
      {
      return mutexOf && hasLock;
      }

   template<class Mutex>
   Mutex* APESEARCH::unique_lock<Mutex>::mutex() const noexcept
      {
      return mutexOf;
      } // end mutex()

   template <class Mutex>
     void APESEARCH::swap(unique_lock<Mutex>& x, unique_lock<Mutex>& y) noexcept
        {
         x.swap( y );
        }