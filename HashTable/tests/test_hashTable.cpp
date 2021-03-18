
#include "../include/HashTable/HashTable.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"
#include <string>
#include <iostream>
#include <algorithm>

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

    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( !kv );
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
TEST( test_find_nothing )
   {
   HashTable<const char*, size_t> hashTable(1);
   Tuple<const char *, size_t> * kv = hashTable.Find( "testing" );
   ASSERT_TRUE( !kv );
   kv = hashTable.Find( "lololol" );
   ASSERT_TRUE( !kv );
   }

static void wrapper( std::vector<std::string>& strings, const size_t val, HashTable<const char*, size_t>& hashTable)
   {
   for ( unsigned n = 0; n < val; ++n )
      {
      strings.emplace_back( std::to_string( n ) );
      const char * ptr = strings[n].c_str();
      hashTable.Find( ptr, n );
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for

   std::vector< const Bucket< const char*, size_t> * > vec( hashTable.constflattenHashTable() );

   std::sort( vec.begin(), vec.end(), 
      []( Bucket< const char*, size_t> const *lhs, Bucket< const char*, size_t> const *rhs ) { return lhs->tuple.value > rhs->tuple.value; } );
   ASSERT_EQUAL( vec.size(), val );

   for ( int n = val - 1, ind = 0; n >= 0; --n, ++ind )
      {
      std::string str = std::to_string( n );
      const Tuple<const char*, size_t>& tuple = vec[(size_t) ind]->tuple;
      ASSERT_EQUAL( tuple.key, std::to_string( n ) );
      ASSERT_EQUAL( tuple.value , n );
      } // end for
   }

TEST( test_constFlattening_full )
   {
   static size_t val = 1000;
   HashTable<const char*, size_t> hashTable(8);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   wrapper( strings, val, hashTable );
   }
TEST( test_constFlattening_sparse )
   {
   static size_t val = 100;
   HashTable<const char*, size_t> hashTable(1024);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   wrapper( strings, val, hashTable );
   }

TEST( test_optimize )
   {
   static size_t val = 1000;
   HashTable<const char*, size_t> hashTable(8);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   wrapper( strings, val, hashTable );

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   hashTable.Optimize();
   ASSERT_EQUAL( strings.size(), hashTable.size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

TEST( test_dont_optimize )
   {
   static size_t val = 4;
   HashTable<const char*, size_t> hashTable(8);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   wrapper( strings, val, hashTable );

   hashTable.Optimize();

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

TEST_MAIN()