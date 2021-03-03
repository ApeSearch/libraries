
#ifndef MUTEX_H_APE_SEARCH
#define MUTEX_H_APE_SEARCH

#include <pthread.h>

namespace APESEARCH
{
class mutex
{

public:
    mutex() noexcept;
    mutex( const mutex& ) = delete;
    mutex& operator=( const mutex& ) = delete;


    void lock();
    bool try_lock();
    void unlock();

    //typedef pthread_mutex_t* native_handle_type;
    pthread_mutex_t* native_handle();

private:
    pthread_mutex_t pThreadLock;
};

struct defer_lock_t { explicit defer_lock_t() = default; };
struct try_to_lock_t { explicit try_to_lock_t() = default; };
struct adopt_lock_t { explicit adopt_lock_t() = default; };

inline constexpr defer_lock_t  defer_lock{};
inline constexpr try_to_lock_t try_to_lock{};
inline constexpr adopt_lock_t  adopt_lock{};

// FOr RAII
//TODO Implement

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
private:
    mutex_type* mutexOf; // Reference to mutex
};

template <class Mutex>
  void swap(unique_lock<Mutex>& x, unique_lock<Mutex>& y) noexcept;


} // end namespace APESEARCH

#include "mutex.inl"

#endif