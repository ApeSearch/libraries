

#include "../include/AS/condition_variable.h"
#include <assert.h>
#include <chrono>
#include <errno.h>
namespace APESEARCH
{

condition_variable::condition_variable()
   {
    if (pthread_cond_init( &cv_construct, NULL ) != 0) 
    {                                  
        perror("pthread_mutex_init() error");                                       
        throw;
    } // end if
   }

condition_variable::~condition_variable()
   {
    pthread_cond_destroy( &cv_construct );
   }

void condition_variable::notify_one() noexcept
   {
    pthread_cond_signal( &cv_construct );
   }

void condition_variable::notify_all() noexcept
   {
    pthread_cond_broadcast( &cv_construct );
   } 

void condition_variable::wait( unique_lock<mutex> &lock )
   {
    assert( lock.owns_lock() );
    pthread_cond_wait( &cv_construct, lock.mutex()->native_handle() );
    assert( lock.owns_lock() );
   } // end wait()

template <class Clock , class Duration>
cv_status condition_variable::wait_until(unique_lock<mutex>& lock,
                   const std::chrono::time_point<Clock, Duration>& abs_time)
   {
    // Get the time now...
    typename Clock::time_point nowEpoch = Clock::now();
    if ( abs_time <= nowEpoch )
        return cv_status::timeout;
    
    std::chrono::time_point<Clock, std::chrono::nanoseconds> inNano = 
       time_point<Clock, std::chrono::nanoseconds>( abs_time.time_since_epoch() );
    
    time_t T;  
    struct timespec t;     
    time( &T );
    t.tv_sec = Clock::now() - inNano;
    if (pthread_cond_timedwait( &cv_construct, lock.mutex()->native_handle() , &t ) != 0)                           
        {
        if (errno == EAGAIN)                                                        
            return cv_status::no_timeout;
        else
            {                                                                      
            perror("pthread_cond_timedwait() error");                                 
            throw;
            } // end else
        } // end if
   } // end wait_until()

pthread_cond_t* condition_variable::native_handle()
   {
    return &cv_construct;
   }


} // end namespace APESEARCH
