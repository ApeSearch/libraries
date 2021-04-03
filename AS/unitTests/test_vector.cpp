#include "dependencies/unit_test_framework.h"
#include "../include/AS/vector.h"
#include <iostream>
#include <vector>

struct President
{
    std::string name;
    std::string country;
    int year;
 
    President(std::string p_name, std::string p_country, int p_year)
        : name(std::move(p_name)), country(std::move(p_country)), year(p_year)
    {
        std::cout << "I am being constructed.\n";
    }
    President(President&& other)
        : name(std::move(other.name)), country(std::move(other.country)), year(other.year)
    {
        std::cout << "I am being moved.\n";
    }
    President& operator=(const President& other)
       {
        name = other.name;
        country = other.country;
        year = other.year;
        std::cout << "Copy constructor is used\n";
        ASSERT_TRUE(false);
        return *this;
       }
};

TEST( std_vector_test )
   {
    std::vector<President> elections;
    std::cout << "emplace_back:\n";
    auto& ref = elections.emplace_back("Nelson Mandela", "South Africa", 1994);
    assert(ref.year != 1984 && "uses a reference to the created object (C++17)");
 
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
   auto& ref = elections.emplace_back( "Nelson Mandela", "South Africa", 1994 );
   ASSERT_EQUAL( ref.year, 1994 );
   ASSERT_EQUAL( ref.name, "Nelson Mandela" );
   ASSERT_EQUAL( ref.country, "South Africa" );

   APESEARCH::vector<President> reElections; 
   std::cout << "\npush_back:\n";
   reElections.push_back( President( "Franklin Delano Roosevelt", "the USA", 1936) );

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

TEST_MAIN()