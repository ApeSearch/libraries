

#include "dependencies/unit_test_framework.h"
#include "../include/AS/string.h"



TEST(test_find)
    {
    string str("eeeeTestinglolTesting2");

    string substr("Testing");

    char * ptrTostr = str.find( substr );

    ASSERT_EQUAL( ptrTostr - str.cstr(), 4 );

    ptrTostr = str.find( "naw" );

    ASSERT_EQUAL( ptrTostr, str.end() );

    //Empty string is a substring of all strings.

    ptrTostr = str.find( "" );
    ASSERT_EQUAL( ptrTostr, str.cstr() );
    }
TEST(test_find_empty)
    {
    string str;

    string substr("Testing");

    char * ptrTostr = str.find( substr );

    ASSERT_EQUAL( ptrTostr, str.end() );

    ptrTostr = str.find( "naw" );

    ASSERT_EQUAL( ptrTostr, str.end() );

    //Empty string is a substring of all strings.
    ptrTostr = str.find( "" );
    ASSERT_EQUAL( ptrTostr, str.cstr() );
    } // end test_find_empty

TEST(fill_constructor)
    {
    string filled(10, 'x');
    ASSERT_EQUAL(filled, "xxxxxxxxxx");
    } // end test fill_constructor

TEST_MAIN()