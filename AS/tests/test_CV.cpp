#include <iostream>
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>
//#include "dependencies/unit_test_framework.h"
#include "../include/AS/condition_variable.h"

using namespace std::chrono_literals;
 
APESEARCH::condition_variable cv;
APESEARCH::mutex cv_m;
std::atomic<int> i{0};
 
 
void waits(int idx)
{
    APESEARCH::unique_lock<APESEARCH::mutex> lk(cv_m);
    auto now = std::chrono::system_clock::now();

    if ( idx == 3 )
       {
        if(cv.wait_until(lk, now + idx*100ms, [](){return i == 1;}))
            std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
        else
            std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
       }
    else
       {
        if(cv.wait_until(lk, now + idx*100ms ) == APESEARCH::cv_status::no_timeout )
            std::cerr << "Thread " << idx << " finished waiting. i == " << i << '\n';
        else
            std::cerr << "Thread " << idx << " timed out. i == " << i << '\n';
       }

    //APESEARCH::cv_status status = cv.wait_until( lk, now + idx*300ms );
    //assert( status == APESEARCH::cv_status::timeout );
}
 
void signals()
{
    std::this_thread::sleep_for(120ms);
    std::cerr << "Notifying...\n";
    cv.notify_all();
    std::this_thread::sleep_for(100ms);
    i = 1;
    std::cerr << "Notifying again...\n";
    cv.notify_all();
}

void *signalsWrapper( void * )
   {
    signals();
    return nullptr;
   }

void *waitsWrapper( void *args )
   {
    int a = *( ( int * ) args );
    free( args );
    waits( a );
    return nullptr;
   }

int main()
{
    std::vector< pthread_t > threads( 4 );
    for ( unsigned n = 0; n < threads.size() - 1; ++n )
       {
        int *arg = ( int * ) malloc( sizeof( *arg ) );
        *arg = (int)n + 1;
        pthread_create( &threads[ n ], 0, waitsWrapper, arg );
       }
    pthread_create( &threads.back(), 0, signalsWrapper, nullptr );
    
    for ( unsigned n = 0; n < threads.size(); ++n )
        pthread_join( threads[n], nullptr );
} // end main()