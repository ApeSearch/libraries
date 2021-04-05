

#pragma once

#ifndef SEMAPHORE_H_APESEARCH
#define SEMAPHORE_H_APESEARCH 

/*
namespace std {

template<ptrdiff_t least_max_value = implementation-defined>
class counting_semaphore
{
public:
static constexpr ptrdiff_t max() noexcept;

constexpr explicit counting_semaphore(ptrdiff_t desired);
~counting_semaphore();

counting_semaphore(const counting_semaphore&) = delete;
counting_semaphore& operator=(const counting_semaphore&) = delete;

void release(ptrdiff_t update = 1);
void acquire();
bool try_acquire() noexcept;
template<class Rep, class Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& rel_time);
template<class Clock, class Duration>
    bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& abs_time);

private:
ptrdiff_t counter; // exposition only
};

using binary_semaphore = counting_semaphore<1>;

}
*/


#include <atomic>
#include <chrono>

#if __cplusplus > 201703L

class __atomic_semaphore_base
{
    std::atomic<std::size_t> __a;

public:
    __atomic_semaphore_base(std::size_t __count) : __a(__count)
    {
    }
    void release(std::size_t __update = 1)
    {
        if(0 < __a.fetch_add(__update, std::memory_order_release))
            ;
        else if(__update > 1)
            __a.notify_all();
        else
            __a.notify_one();
    }
    void acquire()
    {
        /*
        auto const __test_fn = [=]() -> bool {
            ptrdiff_t __old = __a.load(std::memory_order_relaxed);
            return (__old != 0) && __a.compare_exchange_strong(__old, __old - 1, std::memory_order_acquire, std::memory_order_relaxed);
        };
        */
        __a.atomic_wait(__a.load(std::memory_order_relaxed));
    }

    /*

    template <class Rep, class Period>
    bool try_acquire_for(std::chrono::duration<Rep, Period> const& __rel_time)
    {
        auto const __test_fn = [=]() -> bool {
            auto __old = __a.load(memory_order_acquire);
            while(1) {
                if (__old == 0)
                    return false;
                if(__a.compare_exchange_strong(__old, __old - 1, memory_order_acquire, memory_order_relaxed))
                    return true;
            }
        };
        return __libcpp_thread_poll_with_backoff(__test_fn, __libcpp_timed_backoff_policy(), __rel_time);
    }
    */
};
#endif

//Uses native semaphores

#include <semaphore.h>
#include <iostream>
#include <limits>

#define SEMAPHORE_MAX (std::numeric_limits<std::size_t>::max())

class semaphore
{
#ifdef MACOS
    sem_t *pSema;
#else
    sem_t pSema;
#endif

public:
    semaphore(std::size_t __count)
        {
        #ifdef MACOS
            if ( ( pSema = sem_open( "/s", O_CREAT, 0645, __count ) ) == SEM_FAILED )
            {
                std::cerr << "sem_open() failed, errono: " << errno << '\n';
            }
            
            sem_unlink( "/s" );
        #else 
            sem_init( &pSema, 0, (unsigned) __count );
        #endif
        }
    inline ~semaphore() 
        {
        #if MACOS
            sem_close( pSema );
        #else
            sem_destroy( &pSema );
        #endif
        }
    inline void release( std::size_t __update = 1 )
        {
        for(; __update; --__update)
            {
            #ifdef MACOS
               sem_post( pSema );
            #else
               sem_post( &pSema );
            #endif
            }
        }
    void acquire()
        {
        #ifdef MACOS
           sem_wait( pSema );
        #else
           sem_wait( &pSema );
        #endif
        }
        //    
        // bool try_acquire_for(std::chrono::nanoseconds __rel_time)
        // {
        //     return __libcpp_semaphore_wait_timed(&__semaphore, __rel_time);
        // }
};

/*
#if __cplusplus > 201703L
template<ptrdiff_t __least_max_value>
using __semaphore_base =
    __atomic_semaphore_base;
#else
#include<type_traits>
template<ptrdiff_t __least_max_value>
using __semaphore_base =
    typename conditional<(__least_max_value > 1 && __least_max_value <= SEMAPHORE_MAX),
                         semaphore, __atomic_semaphore_base>::type;

#endif

template<ptrdiff_t __least_max_value = SEMAPHORE_MAX>
class counting_semaphore
{
    __semaphore_base<__least_max_value> __semaphore;

public:
    static constexpr ptrdiff_t max() noexcept {
        return __least_max_value;
    }

    counting_semaphore(ptrdiff_t __count = 0) : __semaphore(__count) { }
    ~counting_semaphore() = default;

    counting_semaphore(const counting_semaphore&) = delete;
    counting_semaphore& operator=(const counting_semaphore&) = delete;

    void release(ptrdiff_t __update = 1)
        {
        __semaphore.release(__update);
        }

    void acquire()
        {
        __semaphore.acquire();
        }
    template<class Rep, class Period>
    bool try_acquire_for(std::chrono::duration<Rep, Period> const& __rel_time)
        {
        return __semaphore.try_acquire_for(std::chrono::duration_cast<std::chrono::nanoseconds>(__rel_time));
        }
    bool try_acquire()
        {
        return try_acquire_for(std::chrono::nanoseconds::zero());
        }
    template <class Clock, class Duration>
    bool try_acquire_until(std::chrono::time_point<Clock, Duration> const& __abs_time)
        {
        auto const current = Clock::now();
        if(current >= __abs_time)
            return try_acquire();
        else
            return try_acquire_for(__abs_time - current);
        }
};

using binary_semaphore = counting_semaphore<1>;
*/

#endif