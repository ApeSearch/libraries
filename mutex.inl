


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
unique_lock<Mutex>::unique_lock() noexcept : mutexOf(nullptr) {}


template<class Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m) : mutexOf( &m )
   {
    mutexOf->lock();
   }
// does not acquire ownership of the mutex
template<class Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m, defer_lock_t) noexcept : mutexOf( &m ) 
   {
   }

// tries to acquire ownership of the mutex without blocking
template<class Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m, try_to_lock_t) : mutexOf( &m )
   {
    mutexOf->try_lock();
   }

// assume the calling thread already has ownership of the mutex
template<class Mutex>
unique_lock<Mutex>::unique_lock(mutex_type& m, adopt_lock_t) : mutexOf( &m ) 
   {
   }

template<class Mutex>
unique_lock<Mutex>::~unique_lock()
   {
    mutexOf->unlock();
   }

} // end namespace APESEARCH