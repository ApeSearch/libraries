
#include "dependencies/unit_test_framework.h"
#include "../include/AS/circular_buffer.h"
#include "../include/AS/string.h"
#include <iostream>
using std::cout;


template<typename T, class buffer_type>
static void print_buffer_status(const APESEARCH::circular_buffer<T, buffer_type>& cbuf)
   {
    cout << "Full: " << std::boolalpha << cbuf.full() << ", ";
    cout << "empty: " << cbuf.empty() << ", ";
    cout << "size: " << cbuf.size() << '\n';
    cout << cbuf << '\n';
   }

TEST(A_size_edge_case){

    APESEARCH::circular_buffer<size_t> cbuf;
    ASSERT_TRUE( cbuf.empty() );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 0u );
    // Fill to the max - 1
    for (size_t n = 0; n < 19; ++n)
        cbuf.push_back( n );

    ASSERT_FALSE( cbuf.full() );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 19u );
    ASSERT_EQUAL( cbuf.size(), 19u );
    cbuf.push_back( 20u );
    ASSERT_TRUE( cbuf.full() );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 0u );
    ASSERT_EQUAL( cbuf.size(), 20u );


    // Pop half
    for (size_t n = 0; n < 10; ++n)
       cbuf.pop_front();

    print_buffer_status( cbuf );
    ASSERT_FALSE( cbuf.full() );
    ASSERT_EQUAL( cbuf.getHead(), 10u );
    ASSERT_EQUAL( cbuf.getTail(), 0u );
    ASSERT_EQUAL( cbuf.size(), 10u );
}

TEST(PUT_OVERWRITE_WHILE_FULL){
    APESEARCH::circular_buffer<size_t> cbuf;
    // Fill to the max
    for (size_t n = 0; n < 20; ++n)
        cbuf.push_back( n );
    // Fill again (while max)
    for (size_t n = 0; n < 20;)
       {
        cbuf.putOverwrite( n++ );
        ASSERT_TRUE( cbuf.full() );
        ASSERT_EQUAL( cbuf.getHead(), n % 20 );
        ASSERT_EQUAL( cbuf.getTail(), n % 20 );
        ASSERT_EQUAL( cbuf.size(), 20u );
       }    
    ASSERT_TRUE( cbuf.full() );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 0u );
    ASSERT_EQUAL( cbuf.size(), 20u );
}

TEST(Custom_size) {
    APESEARCH::circular_buffer<size_t, APESEARCH::DEFAULT::defaultBuffer<size_t, 40u>> cbuf;
    ASSERT_TRUE( cbuf.empty() );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 0u );

    for( size_t n = 0; n < 41u; ++n )
       {
         cbuf.push_back( n );
       }
    ASSERT_TRUE( cbuf.full() );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 0u );
    ASSERT_EQUAL( cbuf.size(), 40u );
}
struct President
{
    APESEARCH::string name;
    APESEARCH::string country;
    int year;
 
    President() = default;
    President(APESEARCH::string p_name, APESEARCH::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {
        std::cout << "I am being constructed.\n";
    }
    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {
        std::cout << "I am being moved.\n";
    }
    President& operator=(President&& other)
       {
        std::cout << "Move operator being run...\n";
        name =  std::move( other.name );
        country = std::move( other.country );
        year = other.year;
        return *this;
       }
    President& operator=(const President& other)
       {
        std::cout << "Copy constructor is used\n";
        ASSERT_TRUE(false);
        name = other.name;
        country = other.country;
        year = other.year;
        return *this;
       }
};

TEST( test_emplace )
   {
    APESEARCH::circular_buffer<President> cbuf;
    cbuf.emplace( President( "Franklin Delano Roosevelt", "the USA", 1936 )  );
    ASSERT_EQUAL( cbuf.getHead(), 0u );
    ASSERT_EQUAL( cbuf.getTail(), 1u );
    ASSERT_EQUAL( cbuf.size(), 1u );
    ASSERT_EQUAL( cbuf.front().name, "Franklin Delano Roosevelt" );
    ASSERT_EQUAL( cbuf.front().country, "the USA" );
    ASSERT_EQUAL( cbuf.front().year, 1936 );

    cbuf.emplace( "Barack Obama", "United States", 2008 );

   }


TEST_MAIN()