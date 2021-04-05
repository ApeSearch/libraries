#include "dependencies/unit_test_framework.h"
#include "../include/AS/vector.h"
#include "../include/AS/string.h"
#include <iostream>
#include <vector>

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
        ASSERT_TRUE(false);
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

TEST( std_vector_test )
   {
    std::vector<President> elections;
    std::cout << "emplace_back:\n";
    elections.emplace_back( President( "Nelson Mandela", "South Africa", 1994 ) ); // Will construct and move
#if __cplusplus >= 201703L
    auto& ref = elections.emplace_back("Nelson Mandela", "South Africa", 1994);
#else
    elections.emplace_back("Nelson Mandela", "South Africa", 1994);
    auto& ref = elections.back();
#endif

    ASSERT_EQUAL( ref.name, "Nelson Mandela" );
    elections.emplace_back( "Barack Obama", "United States", 2008 );
 
    std::vector<President> reElections;
    std::cout << "\npush_back:\n";
    reElections.push_back(President("Franklin Delano Roosevelt", "the USA", 1936));
 
    std::cout << "\nContents:\n";
    for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
    for (President const& president: reElections) {
        std::cout << president.name << " was re-elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
   }

TEST( vector_test )
   {
   APESEARCH::vector<President> elections; 
   std::cout << "emplace_back:\n";
   for ( unsigned n = 0; n < 9; ++ n )
      elections.emplace_back( "Barack Obama", "United States", 2008 );
   auto& ref = elections.emplace_back( "Nelson Mandela", "South Africa", 1994 );
   ASSERT_EQUAL( ref.year, 1994 );
   ASSERT_EQUAL( ref.name, "Nelson Mandela" );
   ASSERT_EQUAL( ref.country, "South Africa" );

   APESEARCH::vector<President> reElections; 
   std::cout << "\npush_back:\n";

   reElections.push_back( President( "Franklin Delano Roosevelt", "the USA", 1936) );
   reElections.push_back( President( "Franklin Delano Roosevelt", "the USA", 1936) );
   auto& ref1 = reElections.back();
   ASSERT_EQUAL( ref1.year, 1936 );
   ASSERT_EQUAL( ref1.name, "Franklin Delano Roosevelt" );
   ASSERT_EQUAL( ref1.country, "the USA" );
   std::cout << "\nContents:\n";

   for (President const& president: elections) {
        std::cout << president.name << " was elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
    for (President const& president: reElections) {
        std::cout << president.name << " was re-elected president of "
                  << president.country << " in " << president.year << ".\n";
    }
   }

TEST( test_copy_constructor )
   {
    APESEARCH::vector<APESEARCH::string> strings; 
    for ( unsigned n = 0; n < 100; ++n )
        strings.emplace_back( "testing" );
    APESEARCH::vector<APESEARCH::string> copy( strings ); 

    ASSERT_EQUAL( strings.size(), copy.size() );
   }

TEST( test_move_constructor )
   {
    APESEARCH::vector<APESEARCH::string> strings; 
    for ( unsigned n = 0; n < 100; ++n )
        strings.emplace_back( "testing" );
    APESEARCH::vector<APESEARCH::string> moved( std::move( strings ) ); 

    ASSERT_EQUAL( strings.size(), 0 );
    ASSERT_EQUAL( moved.size(), 100 );
   }

TEST( test_move_operator )
   {
    APESEARCH::vector<APESEARCH::string> strings; 
    for ( unsigned n = 0; n < 100; ++n )
        strings.emplace_back( "testing" );
    APESEARCH::vector<APESEARCH::string> moved; 
    moved = std::move( strings );

    ASSERT_EQUAL( strings.size(), 0 );
    ASSERT_EQUAL( moved.size(), 100 );
   }

TEST( test_init_list_constructor )
    {
    APESEARCH::vector<int> initializer_list_test = 
        {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
        };
    for (int i = 0; i < 10; ++i)
        {
        std::cout << i << "!=" << initializer_list_test[i] << std::endl;
        ASSERT_EQUAL(i, initializer_list_test[i]);
        }
    }

TEST_MAIN()