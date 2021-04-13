
#include "dependencies/unit_test_framework.h"
#include "../include/AS/as_semaphore.h"
#include "../include/AS/mutex.h"
#include "../include/AS/unique_ptr.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <iostream>

struct args
   {
    std::stringstream &ss;
    APESEARCH::semaphore& sema;

    args( std::stringstream& _ss, APESEARCH::semaphore& _sema ) : ss( _ss ), sema( _sema ) {}
   };

static unsigned val = 0;
APESEARCH::mutex coutlk;


static void *thread( void *arg )
   {
    args * argument = reinterpret_cast<args *>( arg );
    coutlk.lock();
    unsigned id = val++;
    std::cout << "Num: " <<  val << std::endl;
    coutlk.unlock();
    argument->sema.down();
    coutlk.lock();
    std::cout << id << " acquired semaphore " << std::endl;
    coutlk.unlock();
    argument->ss << "\nEntered..\n";

    sleep( 6 );


    argument->ss << "\nJust Exiting...\n";
    argument->sema.up();
 
    return nullptr;
   }

TEST( test_native_semaphore )
   {
    std::stringstream strStream;
    APESEARCH::semaphore sema( 0 );
    pthread_t t1, t2;
    APESEARCH::unique_ptr< args > argument( new args( strStream, sema ) );
    void * convArgs = (void * )argument.get();
    pthread_create( &t1, NULL, thread, convArgs );
    sleep( 2 );
    sema.up();
    pthread_create( &t2, NULL, thread, convArgs );
    pthread_join( t1, NULL );
    pthread_join( t2, NULL );

    std::string expectedVal( "\nEntered..\n\nJust Exiting...\n\nEntered..\n\nJust Exiting...\n" );

    ASSERT_EQUAL( strStream.str(), expectedVal );

   }

TEST_MAIN()
