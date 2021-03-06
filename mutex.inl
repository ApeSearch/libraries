


/*
template <class Mutex>
class unique_lock
{
public:
    typedef Mutex mutex_type;
    unique_lock() noexcept;
    explicit unique_lock(mutex_type& m);
    unique_lock(mutex_type& m, defer_lock_t) noexcept;
    unique_lock(mutex_type& m, try_to_lock_t);
    unique_lock(mutex_type& m, adopt_lock_t);

    ~unique_lock();

    unique_lock(unique_lock const&) = delete;
    unique_lock& operator=(unique_lock const&) = delete;

    unique_lock(unique_lock&& u) noexcept;
    unique_lock& operator=(unique_lock&& u) noexcept;

    void lock();
    bool try_lock();

    void unlock();

    void swap(unique_lock& u) noexcept;
    mutex_type* release() noexcept;

    bool owns_lock() const noexcept;
    explicit operator bool () const noexcept;
    mutex_type* mutex() const noexcept;
};

template <class Mutex>
  void swap(unique_lock<Mutex>& x, unique_lock<Mutex>& y) noexcept;
*/

namespace APESEARCH
{
template<class Mutex>
unique_lock<Mutex>::unique_lock() noexcept : mutexOf( nullptr ), hasLock( false ) {}


template<class Mutex>
unique_lock<Mutex>::unique_lock(Mutex& m) : mutexOf( &m )
   {
   lock();
   }
// does not acquire ownership of the mutex
template<class Mutex>
unique_lock<Mutex>::unique_lock(Mutex& m, APESEARCH::defer_lock_t) noexcept : mutexOf( &m ) 
   {
   }

// tries to acquire ownership of the mutex without blocking
template<class Mutex>
unique_lock<Mutex>::unique_lock(Mutex& m, APESEARCH::try_to_lock_t) : mutexOf( &m ), hasLock( try_lock() )
   {
   }

// assume the calling thread already has ownership of the mutex
template<class Mutex>
unique_lock<Mutex>::unique_lock(Mutex& m, APESEARCH::adopt_lock_t) : mutexOf( &m ), hasLock( true )
   {
   }

template<class Mutex>
unique_lock<Mutex>::~unique_lock()
   {
    if ( mutexOf && hasLock )
       mutexOf->unlock();
   }


template<class Mutex>
unique_lock<Mutex>::unique_lock( unique_lock<Mutex>&& u ) noexcept : mutexOf( nullptr )
   {
    // Assumes that the lock is already acquired
    u.swap( *this );
   }

template<class Mutex>
unique_lock<Mutex>& unique_lock<Mutex>::operator=( unique_lock<Mutex>&& u ) noexcept
   {
   unique_lock<Mutex> temp;
   APESEARCH::swap( u, temp );
   temp.swap( *this ); // swap what was taken from temp (allows temp to do whatever freeing is necessary)
   return *this;
   }

template<class Mutex>
inline void unique_lock<Mutex>::lock()
   {
   assert( !hasLock );
   assert( mutexOf );
   mutexOf->lock();
   hasLock = true;
   }

template<class Mutex>
inline bool unique_lock<Mutex>::try_lock()
   {
   assert( mutexOf );
   if ( owns_lock() )
      {
      throw std::system_error( EDEADLK, std::generic_category(), "Attempting to try_lock when lock is already owned");
      }
   return hasLock = mutexOf->try_lock();
   }
   
template<class Mutex>
void unique_lock<Mutex>::unlock()
   {
   assert( hasLock );
   assert( mutexOf );
   mutexOf->unlock();
   hasLock = false;
   }

template<class Mutex>
void unique_lock<Mutex>::swap( unique_lock<Mutex>& u ) noexcept
  {
   APESEARCH::swap( mutexOf, u.mutexOf );
   APESEARCH::swap( hasLock, u.hasLock );
  } // end swap()

template<class Mutex>
Mutex* unique_lock<Mutex>::release() noexcept
   {
   mutex_type *ptr = nullptr;
   APESEARCH::swap( ptr, mutexOf );
   return ptr;
   } // end release()

template<class Mutex>
inline bool unique_lock<Mutex>::owns_lock() const noexcept
   {
   return unique_lock<Mutex>::operator bool () ;
   } // end owns_lock()

template<class Mutex>
unique_lock<Mutex>::operator bool () const noexcept
   {
   return mutexOf && hasLock;
   }

template<class Mutex>
Mutex* unique_lock<Mutex>::mutex() const noexcept
   {
   return mutexOf;
   } // end mutex()

template <class Mutex>
  void swap(unique_lock<Mutex>& x, unique_lock<Mutex>& y) noexcept
     {
      x.swap( y );
     }

} // end namespace APESEARCH
