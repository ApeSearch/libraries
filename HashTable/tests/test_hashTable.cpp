
#include "../include/HashTable/HashTable.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"

TEST( test_find )
   {
    HashTable<const char*, size_t> hashTable;
    ASSERT_EQUAL(hashTable.Find( "testing" ), nullptr);
    ASSERT_EQUAL( hashTable.Find( "testing", 100 )->value, 100 );
    Tuple<const char *, size_t> * kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );

   }


TEST_MAIN()