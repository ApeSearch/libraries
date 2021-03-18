
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

   }


TEST_MAIN()