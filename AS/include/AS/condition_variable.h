
#pragma once

#ifndef CONDITION_VARIABLE_H_APESEARCH
#define CONDITION_VARIABLE_H_APESEARCH

#include <pthread.h>
#include "mutex.h"
#include <condition_variable>
#include <chrono>

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
                   const std::chrono::time_point<Clock, Duration>& abs_time);
/*

    template <class Clock, class Duration, class Predicate>
        bool
        wait_until(unique_lock<mutex>& lock,
                   const chrono::time_point<Clock, Duration>& abs_time,
                   Predicate pred);

    template <class Rep, class Period>
        cv_status
        wait_for(unique_lock<mutex>& lock,
                 const chrono::duration<Rep, Period>& rel_time);

    template <class Rep, class Period, class Predicate>
        bool
        wait_for(unique_lock<mutex>& lock,
                 const chrono::duration<Rep, Period>& rel_time,
                 Predicate pred);
*/
    typedef pthread_cond_t* native_handle_type;
    native_handle_type native_handle();
};

//void notify_all_at_thread_exit(condition_variable& cond, unique_lock<mutex> lk);



} // end namespace




#endif // end CONDITION_VARIABLE_H_APESEARCH