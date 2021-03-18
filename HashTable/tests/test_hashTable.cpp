
#include "../include/HashTable/HashTable.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"

TEST( test_find )
   {
    HashTable<const char*, size_t> hashTable(8);
    ASSERT_EQUAL(hashTable.Find( "testing" ), nullptr);
    Tuple<const char *, size_t> * kv = hashTable.Find( "testing", 100 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );

    kv = hashTable.Find( "lololol", 42 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );

    // Now check previous values...
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );
   }

TEST( test_find_seperate_chaining )
   {
   HashTable<const char*, size_t> hashTable(1);
   // Insert "testing"
   ASSERT_EQUAL(hashTable.Find( "testing" ), nullptr);
    Tuple<const char *, size_t> * kv = hashTable.Find( "testing", 100 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );

   // insert "lololol"
    kv = hashTable.Find( "lololol", 42 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );

   // insert "1 2 3 3 4 5 "
    kv = hashTable.Find( "1 2 3 3 4 5 ", 666 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 666 );
    kv = hashTable.Find( "1 2 3 3 4 5 " );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 666 );

    // Now check previous values...
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );

   }


TEST_MAIN()