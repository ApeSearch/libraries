

#include "../include/AS/mutex.h"
#include <stdio.h> // for perror()

//------------------------------------------------------------------------------------------------
//
//                                  Mutex Definition 
//
//------------------------------------------------------------------------------------------------


APESEARCH::mutex::mutex() noexcept
    {
    if (pthread_mutex_init( &pThreadLock, nullptr ) != 0 )
       perror("Pthread_mutex_init failed:") ;
    } // end mutex()

APESEARCH::mutex::~mutex()
   {
   pthread_mutex_destroy( &pThreadLock );
   }


void APESEARCH::mutex::lock()
   {
   pthread_mutex_lock( &pThreadLock ); 
   }

bool APESEARCH::mutex::try_lock()
   {
   return !pthread_mutex_trylock( &pThreadLock ); 
   }

void APESEARCH::mutex::unlock()
   {
   pthread_mutex_unlock( &pThreadLock );    
   }

pthread_mutex_t* APESEARCH::mutex::native_handle()
   {
   return &pThreadLock; 
   }



