#include "../include/AS/mutex.h"
using APESEARCH::mutex;

#include <pthread.h>

#include <iostream>
using std::cout;
using std::endl;

#define NUMTHREADS 100

mutex coutLock;

void *testingMutex(void *)
{
    coutLock.lock();
    cout << "testing that output isn't garbage" << endl;    
    coutLock.unlock();
    return nullptr;
}

int main()
{
    pthread_t array[NUMTHREADS];

    for (size_t n = 0; n < NUMTHREADS; ++n)
    {
        if (pthread_create( &array[n], nullptr, testingMutex, nullptr) != 0)
        {
            perror("thread_create(0 error");
            return 1;
        } // end pthread
    }


    for(size_t n = 0; n < NUMTHREADS; ++n)
    {
        pthread_join(array[n], nullptr);
    }
} // end main()

