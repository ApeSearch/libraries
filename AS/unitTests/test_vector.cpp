#include "dependencies/unit_test_framework.h"
#include "../include/AS/vector.h"

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
    President& operator=(const President& other) = default;
};



TEST( vector_test )
   {
   APESEARCH::vector<President> elections; 
   auto& ref = elections.emplace_back( "Nelson Mandela", "South Africa", 1994 );
   ASSERT_EQUAL( ref.year, 1994 );
   ASSERT_EQUAL( ref.name, "Nelson Mandela" );
   ASSERT_EQUAL( ref.country, "South Africa" );
   }

TEST_MAIN()