
#include "dependencies/unit_test_framework.h"
#include "../include/AS/mutex.h"
using APESEARCH::unique_lock;
using APESEARCH::mutex;
#include <iostream>

static APESEARCH::mutex lock;

TEST(DEFAULT_Constructor){ 
{
    unique_lock<mutex> uniqLk;
    // implicitly testing bool as well
    ASSERT_FALSE( uniqLk ? true : false );
}
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();

} // end Test(constructors)

TEST(locking_constructor) {
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock);
    // implicitly testing bool as well
    ASSERT_TRUE( uniqLk ? true : false );
    ASSERT_TRUE( uniqLk.owns_lock() );
}
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(Defer_constructor) {
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock, APESEARCH::defer_lock);
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_TRUE( uniqLk.try_lock() );
}
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(Try_to_lock_constructor) {
{   // Try to lock when lock not owned
    ASSERT_TRUE(lock.try_lock());
    lock.unlock(); 
    unique_lock<mutex> uniqLk(lock, APESEARCH::try_to_lock);
    ASSERT_TRUE( uniqLk.owns_lock() );
}
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();

{   // Try to lock when lock is already owned
    lock.lock(); // lock beforehand
    unique_lock<mutex> uniqLk(lock, APESEARCH::try_to_lock);
    ASSERT_FALSE( uniqLk.owns_lock() );
}
{ // Adopt lock
    unique_lock<mutex> uniqLk(lock, APESEARCH::adopt_lock);
    ASSERT_TRUE( uniqLk.owns_lock() );
} // lock.unlock() is implicitly run
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(A_Swap) {
{   // Try to lock when lock not owned
    ASSERT_TRUE(lock.try_lock());
    lock.unlock(); 
    unique_lock<mutex> uniqLk(lock);
    unique_lock<mutex> moveToUniqLk;
    ASSERT_TRUE( uniqLk.owns_lock() );
    ASSERT_FALSE( moveToUniqLk.owns_lock() );

    // Swap
    uniqLk.swapUM( moveToUniqLk );
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_TRUE( moveToUniqLk.owns_lock() );
}
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();

{   // Try to lock when lock not owned
    ASSERT_TRUE(lock.try_lock());
    lock.unlock(); 
    unique_lock<mutex> uniqLk(lock);
    unique_lock<mutex> moveToUniqLk;
    ASSERT_TRUE( uniqLk.owns_lock() );
    ASSERT_FALSE( moveToUniqLk.owns_lock() );

    // Swap
    APESEARCH::swap( uniqLk, moveToUniqLk );
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_TRUE( moveToUniqLk.owns_lock() );
}
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();

}


TEST(Move_constructor) {
    {   // Try to lock when lock not owned
    ASSERT_TRUE(lock.try_lock());
    lock.unlock(); 
    unique_lock<mutex> uniqLk(lock);
    ASSERT_TRUE( uniqLk.owns_lock() );

    // Move constructor
    unique_lock<mutex> moveToUniqLk( std::move( uniqLk ) );
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_TRUE( moveToUniqLk.owns_lock() );
    }
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(Move_operator) {
    {   // Try to lock when lock not owned
    ASSERT_TRUE(lock.try_lock());
    lock.unlock(); 
    unique_lock<mutex> uniqLk(lock);
    unique_lock<mutex> moveToUniqLk;
    ASSERT_TRUE( uniqLk.owns_lock() );
    ASSERT_FALSE( moveToUniqLk.owns_lock() );

    // Move operator
    moveToUniqLk = std::move( uniqLk );
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_TRUE( moveToUniqLk.owns_lock() );
    }
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(LOCK) {
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock, APESEARCH::defer_lock);
    ASSERT_FALSE( uniqLk.owns_lock() );
    uniqLk.lock();
    ASSERT_TRUE( uniqLk.owns_lock() );
}
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(TRYLOCK) {
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock, APESEARCH::defer_lock);
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_TRUE(uniqLk.try_lock());
    ASSERT_TRUE( uniqLk.owns_lock() );
}
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
{   // Try to lock when lock is already acquired
    unique_lock<mutex> uniqLk(lock, APESEARCH::defer_lock);
    ASSERT_FALSE( uniqLk.owns_lock() );
    ASSERT_FALSE(uniqLk.try_lock());
    ASSERT_FALSE( uniqLk.owns_lock() );
}
    lock.unlock();
}

TEST(UNLOCK) {
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock);
    ASSERT_TRUE( uniqLk.owns_lock() );
    uniqLk.unlock();
    ASSERT_FALSE( uniqLk.owns_lock() );
}
    // Check is unlocked
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(RELEASE) {
    mutex *mutex_ptr;
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock);
    ASSERT_TRUE( uniqLk.owns_lock() );
    mutex_ptr = uniqLk.release();
    ASSERT_EQUAL(mutex_ptr, &lock);
}
    ASSERT_FALSE(lock.try_lock());
    mutex_ptr->unlock();
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}

TEST(mutex_ptr) {
    mutex *mutex_ptr;
{
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
    unique_lock<mutex> uniqLk(lock);
    ASSERT_TRUE( uniqLk.owns_lock() );
    mutex_ptr = uniqLk.mutex();
    ASSERT_EQUAL(mutex_ptr, &lock);
}
    ASSERT_TRUE(lock.try_lock());
    lock.unlock();
}



TEST(Owns_lock) {

    unique_lock<mutex> uniqLk(lock);
    try 
    {
        uniqLk.try_lock();
        ASSERT_TRUE(false);
    }
    catch ( const std::system_error& ex )
       {
       ASSERT_TRUE(true);
       }
    catch (...)
       {
        ASSERT_TRUE(false);
       }
}

TEST_MAIN()
