#include "../include/HashTable/HashTable.h"
#include "../include/HashTable/HashBlob.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"

static void testingFind( HashTable<const char*, size_t>& hashTable );

TEST( test_BytesForHeaderBuckets )
   {
    
    HashTable<const char*, size_t> hashTable(8);
    testingFind( hashTable ); // Insert into hashTable...

    size_t bytesReq = HashBlob::BytesForHeaderBuckets( &hashTable ); 
    size_t expectedBytes = ( 4 + hashTable.table_size() ) * sizeof( size_t ); // 4 * 8ish
    ASSERT_EQUAL( bytesReq, expectedBytes );
    
   }


void testingFind( HashTable<const char*, size_t>& hashTable )
   {
   // Insert "testing"
   ASSERT_EQUAL(hashTable.Find( "testing" ), nullptr);
    Tuple<const char *, size_t> * kv = hashTable.Find( "testing", 100 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100 );

   // insert "lololol"
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( !kv );
    kv = hashTable.Find( "lololol", 42 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42 );

   // insert "1 2 3 3 4 5 "
    kv = hashTable.Find( "1 2 3 3 4 5 " );
    ASSERT_TRUE( !kv );
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