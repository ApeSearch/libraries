
#pragma once

#ifndef CONDITION_VARIABLE_H_APESEARCH
#define CONDITION_VARIABLE_H_APESEARCH

#include <pthread.h>
#include "mutex.h"
#include <chrono>
#include <errno.h> // for errno

namespace APESEARCH
{

enum class cv_status { no_timeout, timeout };

class condition_variable
{
   pthread_cond_t cv_construct;

public:
    condition_variable();
    ~condition_variable();

    condition_variable(const condition_variable&) = delete;
    condition_variable& operator=(const condition_variable&) = delete;

    void notify_one() noexcept;
    void notify_all() noexcept;

    void wait(unique_lock<mutex>& lock);
    template <class Predicate>
    void wait(unique_lock<mutex>& lock, Predicate pred)
        {
        while( !pred() )
           wait( lock );
        } // end wait()

    template <class Clock, class Duration>
        cv_status
        wait_until(unique_lock<mutex>& lock,
                   const std::chrono::time_point<Clock, Duration>& abs_time)
        {
        // Get the time now...
        typename Clock::time_point nowEpoch = Clock::now();
        // If passed than no need
        if ( abs_time <= nowEpoch )
            return cv_status::timeout;
        
        std::chrono::time_point<Clock, std::chrono::nanoseconds> inNano = 
        std::chrono::time_point<Clock, std::chrono::nanoseconds>( abs_time.time_since_epoch() );

        //auto secs = duration_cast<std::chrono::seconds>( inNano - Clock::now() );
        //auto ns = duration_cast<std::chrono::seconds>( inNano - secs );

        //std::chrono::nanoseconds ns = inNano - Clock::now();
        //auto secs = std::chrono::duration_cast< std::chrono::seconds >( ns ); // truncate nanoseconds
        //ns -= secs; // get it back

        auto secs = std::chrono::time_point_cast< std::chrono::seconds >( inNano );
        auto ns = std::chrono::time_point_cast< std::chrono::nanoseconds >( inNano ) - 
            std::chrono::time_point_cast< std::chrono::nanoseconds >( secs );


        struct timespec t{ secs.time_since_epoch().count(), ns.count() };     
        int rt;
        if ( ( rt = pthread_cond_timedwait( &cv_construct, lock.mutex()->native_handle() , &t ) ) < 0 )                           
            {
            switch( errno )
            {
            case EAGAIN:
                return cv_status::timeout;
            case ETIMEDOUT:
                return cv_status::timeout;
            case EINVAL:
                perror("pthread_cond_timedwait() error");        
                throw std::runtime_error( "pthread_cond_timedwait() error: EINVAL" );
            default:
                perror("pthread_cond_timedwait() error");                                 
                printf( "Val: %d\n", errno );
            throw std::runtime_error( "pthread_cond_timedwait() error" );
            }
            } // end if
        return cv_status::no_timeout;
        }

    template <class Clock, class Duration, class Predicate>
        bool
        wait_until(unique_lock<mutex>& lock,
                const std::chrono::time_point<Clock, Duration>& abs_time,
                Predicate pred)
        {
        {
        while( !pred() )
            {
            if ( wait_until( lock, abs_time ) == cv_status::timeout )
                return pred();

            } // end while
        return true;
        }
        }
    template <class Rep, class Period>
    cv_status wait_for(unique_lock<mutex>& lock,
                const std::chrono::duration<Rep, Period>& rel_time)
        {
        return wait_until( lock, std::chrono::steady_clock::now() + rel_time );
        }

    template <class Rep, class Period, class Predicate>
    bool wait_for(unique_lock<mutex>& lock,
                 const std::chrono::duration<Rep, Period>& rel_time,
                 Predicate pred)
            {
            return wait_until( lock, std::chrono::steady_clock::now() + rel_time, pred );
            }
    typedef pthread_cond_t* native_handle_type;
    native_handle_type native_handle();
};

//void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);



} // end namespace




#endif // end CONDITION_VARIABLE_H_APESEARCH