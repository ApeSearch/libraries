#include "dependencies/unit_test_framework.h"
#include "../include/AS/queue.h"

TEST( test_0_basic )
   {
    APESEARCH::priority_queue<int> pq;

    pq.push( 3 );
    ASSERT_EQUAL( pq.top(), 3 );
    pq.push( 4 );
    ASSERT_EQUAL( pq.top(), 4 );
    ASSERT_EQUAL( pq.size(), 2u );

    pq.pop();
    ASSERT_EQUAL( pq.size(), 1u );
    ASSERT_EQUAL( pq.top(), 3 );

    pq.pop();
    ASSERT_EQUAL( pq.size(), 0u );
    ASSERT_TRUE( pq.empty() );

   }

TEST( test_1 )
   {
    APESEARCH::priority_queue<int> pq;
    std::cerr << "Starting test 1" << std::endl;
    pq.push(150);
    pq.push(4);
    pq.emplace(100);
    pq.emplace(3);
    pq.push(1);
    pq.push(0);
    pq.push(90);
    pq.push(2);
    pq.push(1);
    pq.push(10);
    ASSERT_EQUAL( pq.size(), 10u );
    ASSERT_EQUAL( pq.top(), 150 );
    
    pq.pop();
    ASSERT_EQUAL( pq.top(), 100 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 90 );

    pq.pop();
    ASSERT_EQUAL( pq.top(), 10 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 4 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 3 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 2 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 1 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 1 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 0 );
    pq.pop();
    ASSERT_EQUAL( pq.size(), 0u );
    ASSERT_TRUE( pq.empty() );
   }

TEST( test_3 )
   {
    APESEARCH::priority_queue<int> pq;
    std::cerr << "Starting test 3" << std::endl;
    pq.push(5);
    pq.push(3);
    pq.push(10);
    pq.push(1000);
    ASSERT_EQUAL( pq.size(), 4u );
    ASSERT_EQUAL( pq.top(), 1000 );
    pq.pop();
    pq.push( -100 );
    pq.push( 0 );
    ASSERT_EQUAL( pq.size(), 5u );
    pq.pop();
    pq.pop();
    ASSERT_EQUAL( pq.top(), 3 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), 0 );
    pq.pop();
    ASSERT_EQUAL( pq.top(), -100 );
    pq.pop();
    ASSERT_TRUE( pq.empty() );
   }

TEST_MAIN()