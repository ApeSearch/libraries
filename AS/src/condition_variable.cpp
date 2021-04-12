

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


pthread_cond_t* condition_variable::native_handle()
   {
    return &cv_construct;
   }

} // end namespace APESEARCH
