

#include "dependencies/unit_test_framework.h"
#include "../include/AS/string.h"



TEST(test_find)
    {
    string str("eeeeTestinglolTesting2");

    string substr("Testing");

    char * ptrTostr = str.findPtr( substr );

    ASSERT_EQUAL( ptrTostr - str.cstr(), 4 );

    ptrTostr = str.findPtr( "naw" );

    ASSERT_EQUAL( ptrTostr, str.end() );

    //Empty string is a substring of all strings.

    ptrTostr = str.findPtr( "" );
    ASSERT_EQUAL( ptrTostr, str.cstr() );


    }
TEST(test_find_empty)
    {
    string str;

    string substr("Testing");

    char * ptrTostr = str.findPtr( substr );

    ASSERT_EQUAL( ptrTostr, str.end() );

    ptrTostr = str.findPtr( "naw" );

    ASSERT_EQUAL( ptrTostr, str.end() );

    //Empty string is a substring of all strings.
    ptrTostr = str.findPtr( "" );
    ASSERT_EQUAL( ptrTostr, str.cstr() );
    } // end test_find_empty

TEST( test_cstr )
   {
    string str( "testing" );
    ASSERT_EQUAL( str, "testing" );
   }


TEST( test_findSize_t )
   {
    string str("eeeeTestinglolTesting2");

    string substr("Testing");

    size_t indTostr = str.find( substr );

    ASSERT_EQUAL( indTostr, 4 );

    indTostr = str.find( "naw" );

    ASSERT_EQUAL( indTostr, string::npos );

    //Empty string is a substring of all strings.

    indTostr = str.find( "" );
    ASSERT_EQUAL( indTostr, 0 );
   }

TEST(test_find_empty_size_t)
    {
    string str;

    string substr("Testing");

    size_t indTostr = str.find( substr );

    ASSERT_EQUAL( indTostr, string::npos );

    indTostr = str.find( "naw" );

    ASSERT_EQUAL( indTostr, string::npos );

    //Empty string is a substring of all strings.
    indTostr = str.find( "" );
    ASSERT_EQUAL( indTostr, 0 );
    } // end test_find_empty

TEST( test_substr )
   {
    string str("substringsTesting");
    size_t ind = str.find( "Testing" );
    string substr( str, ind );
    ASSERT_EQUAL( substr, "Testing" );
    //ASSERT_EQUAL( )
    substr = string( str, ind, 1 );
    ASSERT_EQUAL( substr, "T" );
    string strLetter('W'); // As long as explicit OK
    ASSERT_EQUAL( strLetter, "W" );
   }

TEST( test_append )
   {
    string str("Append");
    string strCopy( str );
    ASSERT_EQUAL( str, strCopy );
    string strOther("RandStuff");

    str += strOther;
    ASSERT_NOT_EQUAL( str, strCopy );
    ASSERT_EQUAL( str, "AppendRandStuff" );
    str += "LiteralsAreWack";
    ASSERT_EQUAL( str, "AppendRandStuffLiteralsAreWack" );
   }

TEST( test_compare )
   {
    
    string str = "first";
    string str2 = "second";

    // "first", "second"
    ASSERT_FALSE( str == str2 );
    ASSERT_TRUE( str < str2 );
    ASSERT_TRUE( str <= str2 );

    str2 = "first1";
    // "first", "first1"
    ASSERT_FALSE( str == str2 );
    ASSERT_TRUE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    str2.pop_back();
    // "first", "first"
    ASSERT_TRUE( str == str2 );
    ASSERT_FALSE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    ASSERT_FALSE( str > str2 );
    ASSERT_TRUE( str >= str2 );
    ASSERT_FALSE( str != str2 );

    // "firs", "first"
    str.pop_back();
    ASSERT_FALSE( str == str2 );
    ASSERT_TRUE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    ASSERT_FALSE( str > str2 );
    ASSERT_FALSE( str >= str2 );
    ASSERT_TRUE( str != str2 );
    str.push_back('t');
    // "first", "first"
    ASSERT_TRUE( str == str2 );
    ASSERT_FALSE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    ASSERT_FALSE( str > str2 );
    ASSERT_TRUE( str >= str2 );
    ASSERT_FALSE( str != str2 );
    
    // "first", "first\0"
    str2.push_back('\0');
    ASSERT_FALSE( str == str2 );
    ASSERT_TRUE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    ASSERT_FALSE( str > str2 );
    ASSERT_FALSE( str >= str2 );
    ASSERT_TRUE( str != str2 );
    str.push_back('\0');
    // "first\0", "first\0"
    ASSERT_TRUE( str == str2 );
    ASSERT_FALSE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    ASSERT_FALSE( str > str2 );
    ASSERT_TRUE( str >= str2 );
    ASSERT_FALSE( str != str2 );
    str.push_back('\0');
    // "first\0\0", "first\0"
    ASSERT_FALSE( str == str2 );
    ASSERT_FALSE( str < str2 );
    ASSERT_FALSE( str <= str2 );
    ASSERT_TRUE( str > str2 );
    ASSERT_TRUE( str >= str2 );
    ASSERT_TRUE( str != str2 );


    // "first\0", "first\0f"
    str.pop_back();
    str2.push_back('f');
    ASSERT_FALSE( str == str2 );
    ASSERT_TRUE( str < str2 );
    ASSERT_TRUE( str <= str2 );
    ASSERT_FALSE( str > str2 );
    ASSERT_FALSE( str >= str2 );
    ASSERT_TRUE( str != str2 );

   }

TEST_MAIN()