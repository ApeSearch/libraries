
#include "../include/HashTable/HashTable.h"
#include "../include/HashTable/TopN.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"
#include <string>
#include <iostream>
#include <algorithm>


static void testingFind( HashTable<const char*, size_t>& hashTable );

TEST( test_find )
   {
    HashTable<const char*, size_t> hashTable(8);
    testingFind( hashTable );
   }

TEST( test_find_seperate_chaining )
   {
   HashTable<const char*, size_t> hashTable(1); // Forces all entries into one bucket
   testingFind( hashTable );
   }

void testingFind( HashTable<const char*, size_t>& hashTable )
   {
   // Insert "testing"
   ASSERT_EQUAL(hashTable.Find( "testing" ), nullptr);
    Tuple<const char *, size_t> * kv = hashTable.Find( "testing", 100 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100ul );
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100ul );

   // insert "lololol"
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( !kv );
    kv = hashTable.Find( "lololol", 42 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42ul );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42ul );

   // insert "1 2 3 3 4 5 "
    kv = hashTable.Find( "1 2 3 3 4 5 " );
    ASSERT_TRUE( !kv );
    kv = hashTable.Find( "1 2 3 3 4 5 ", 666 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 666ul );
    kv = hashTable.Find( "1 2 3 3 4 5 " );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 666ul );

    // Now check previous values...
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100ul );
    kv = hashTable.Find( "lololol" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 42ul );
   }

TEST( test_find_nothing )
   {
   HashTable<const char*, size_t> hashTable(1);
   Tuple<const char *, size_t> * kv = hashTable.Find( "testing" );
   ASSERT_TRUE( !kv );
   kv = hashTable.Find( "lololol" );
   ASSERT_TRUE( !kv );
   }

TEST( test_modify )
   {
   HashTable<const char*, size_t> hashTable(128);
   // Insert "testing"
    Tuple<const char *, size_t> *kv = hashTable.Find( "testing", 100 );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 100ul );
    kv->value = 9001;
    kv = hashTable.Find( "testing" );
    ASSERT_TRUE( kv );
    ASSERT_EQUAL( kv->value, 9001ul );
   }

// Testing the issue if ( *bucket )->tuple.key == k 
// is used over CompareEqual( ( *bucket )->tuple.key, k ) 
TEST( test_Comparison )
   {
   HashTable<const char*, size_t> hashTable(128);
   // Insert "testing"
   std::string str1( "testing" );
   std::string str2( "testing" );
   Tuple<const char *, size_t> *kv = hashTable.Find( str1.c_str(), 100 );
   ASSERT_TRUE( kv );
   ASSERT_EQUAL( kv->value, 100ul );
   kv = hashTable.Find( str2.c_str(), 42 );
   ASSERT_TRUE( kv );
   ASSERT_EQUAL( kv->value, 100ul );

   ASSERT_EQUAL( hashTable.size(), 1ul );
   }

static void testingFlattening( std::vector<std::string>& strings, const size_t val, HashTable<const char*, size_t>& hashTable);

TEST( test_constFlattening_full )
   {
   static size_t val = 1000;
   HashTable<const char*, size_t> hashTable(8);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );
   }
TEST( test_constFlattening_sparse )
   {
   static size_t val = 100;
   HashTable<const char*, size_t> hashTable(1024);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );
   }

void testingFlattening( std::vector<std::string>& strings, const size_t val, HashTable<const char*, size_t>& hashTable)
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

   for ( int n = ( int )val - 1, ind = 0; n >= 0; --n, ++ind )
      {
      std::string str = std::to_string( n );
      const Tuple<const char*, size_t>& tuple = vec[(size_t) ind]->tuple;
      ASSERT_EQUAL( tuple.key, std::to_string( n ) );
      ASSERT_EQUAL( tuple.value , (size_t)n );
      } // end for
   }

TEST( test_optimize )
   {
   static size_t val = 10000;
   HashTable<const char*, size_t> hashTable;
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096ul, hashTable.table_size() );
   hashTable.Optimize(); // Current load factor becomes at most 0.5
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   //ASSERT_EQUAL( 32768, hashTable.table_size() );
   //ASSERT_EQUAL( 16384, hashTable.table_size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

TEST( test_optimize_shrink )
   {
   static size_t val = 4;
   HashTable<const char*, size_t> hashTable( 4096 );
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096ul, hashTable.table_size() ); // 4096
   hashTable.Optimize();
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 8ul, hashTable.table_size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

// Check that a bucket size of 3 still results in a table size of 8
TEST( test_optimize_shrink2 )
   {
   static size_t val = 3;
   HashTable<const char*, size_t> hashTable( 4096 );
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096ul, hashTable.table_size() ); // 4096
   hashTable.Optimize();
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   //ASSERT_EQUAL( 8, hashTable.table_size() );
   ASSERT_EQUAL( 4ul, hashTable.table_size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

TEST( test_optimizeElegant )
   {
   static size_t val = 10000;
   HashTable<const char*, size_t> hashTable( 4096 );
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096ul, hashTable.table_size() );
   hashTable.OptimizeElegant(); // Current load factor becomes at most 0.5
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 32768ul, hashTable.table_size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

TEST( test_optimizeElegant_shrink )
   {
   static size_t val = 4;
   HashTable<const char*, size_t> hashTable( 4096 );
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096ul, hashTable.table_size() ); // 4096
   hashTable.OptimizeElegant();
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 8ul, hashTable.table_size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[(size_t)n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, (size_t)n );
      } // end for
   }

// Check that a bucket size of 3 still results in a table size of 8
TEST( test_optimizeElegant_shrink2 )
   {
   static size_t val = 3;
   HashTable<const char*, size_t> hashTable( 4096 );
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096ul, hashTable.table_size() ); // 4096
   hashTable.OptimizeElegant();
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 8ul, hashTable.table_size() );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[(size_t)n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, (size_t)n );
      } // end for
   }

TEST( test_numOfLinkedLists )
   {
   static size_t val = 10;
   HashTable<const char*, size_t> hashTable(1); // purposely absolute worst case for insertion sort
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   for ( unsigned n = 0; n < val; ++n )
      {
      strings.emplace_back( std::to_string( n ) );
      const char * ptr = strings[n].c_str();
      hashTable.Find( ptr, n );
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str(), val );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for

   ASSERT_EQUAL( hashTable.numOfLinkedLists(), 1ul );

   hashTable.Optimize();

   // It's expected that load factor is less than 1
   ASSERT_EQUAL( hashTable.size() - hashTable.getCollisions(), hashTable.numOfLinkedLists() );

   }


TEST( test_power_of_two )
   {
   static size_t val = 16;
   HashTable<const char*, size_t> hashTable;
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );

   ASSERT_EQUAL( hashTable.table_size(), (size_t)DEFAULTSIZE );
   hashTable.Optimize();
   ASSERT_EQUAL( hashTable.table_size(), 32ul );
   //ASSERT_EQUAL( hashTable.table_size(), 16 );

   for ( unsigned n = 0; n < val; ++n )
      {
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   }

TEST( test_power_of_two_general )
   {
   ASSERT_EQUAL( computeTwosPow(1), 1ul );
   ASSERT_EQUAL( computeTwosPow(2), 2ul );
   ASSERT_EQUAL( computeTwosPow(3), 4ul );
   ASSERT_EQUAL( computeTwosPow(16), 16ul );
   ASSERT_EQUAL( computeTwosPow(17), 32ul );
   ASSERT_EQUAL( computeTwosPow(1, false), 1ul );
   ASSERT_EQUAL( computeTwosPow(2, false), 2ul );
   ASSERT_EQUAL( computeTwosPow(3, false), 2ul );
   ASSERT_EQUAL( computeTwosPow(16, false), 16ul );
   ASSERT_EQUAL( computeTwosPow(17, false), 16ul );
   ASSERT_EQUAL( computeTwosPow(65536, false), 65536ul );
   ASSERT_EQUAL( computeTwosPow( 65536 ), 65536ul );
   ASSERT_EQUAL( computeTwosPow( 65535, false ), 32768ul );

   }

TEST( test_iterators )
   {
   HashTable<const char*, size_t> hashTable(1); // Forces all entries into one bucket
   ASSERT_EQUAL(hashTable.Find( "testing" ), nullptr);
   Tuple<const char *, size_t> * kv = hashTable.Find( "testing", 100 );
   kv = hashTable.Find( "lololol", 42 );

   kv = hashTable.Find( "testing" );

   HashTable<const char*, size_t>::iterator itr = hashTable.begin();
   ASSERT_EQUAL( itr->value, 100ul );

   }

TEST( test_iterator_operators )
   {
   static size_t val = 10;
   HashTable<const char*, size_t> hashTable(1); // purposely absolute worst case for insertion sort
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   for ( unsigned n = 0; n < val; ++n )
      {
      strings.emplace_back( std::to_string( n ) );
      const char * ptr = strings[n].c_str();
      hashTable.Find( ptr, n );
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str(), val );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, (size_t)n );
      } // end for

   HashTable<const char*, size_t>::iterator itr = hashTable.begin();

   HashTable<const char*, size_t>::iterator postFix = itr++;
   ASSERT_EQUAL( postFix, hashTable.begin() );

   itr++;
   ASSERT_NOT_EQUAL( itr, hashTable.begin() );
   ASSERT_EQUAL( itr, hashTable.begin() + 2 );

   HashTable<const char*, size_t>::iterator itr2 = hashTable.begin();

   ASSERT_EQUAL( ++itr2, hashTable.begin() + 1 );

   HashTable<const char*, size_t> emptyhashTable(1);
   ASSERT_EQUAL( emptyhashTable.begin(), emptyhashTable.end() );


   HashTable<const char*, size_t>::iterator copy( itr2 );

   ASSERT_NOT_EQUAL( ++copy, itr2 );


   const char *charKey = itr2->key;

   HashTable<const char*, size_t>::iterator foundItr = hashTable.FindItr( charKey );
   ASSERT_EQUAL( foundItr, itr2 );

   size_t oldVal = itr2->value;
   itr2->value *= 2;

   ASSERT_EQUAL( foundItr, itr2 );

   ASSERT_NOT_EQUAL( hashTable.FindItr( charKey )->value, oldVal );

   }

TEST( test_iterating )
   {
   static size_t val = 10;
   HashTable<const char*, size_t> hashTable(8);
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
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

   hashTable.Optimize();
   std::vector< const Bucket< const char*, size_t> * > vec( hashTable.constflattenHashTable() );
   ASSERT_EQUAL( vec.size(), hashTable.size() );

   HashTable<const char*, size_t>::iterator hTItr = hashTable.begin();
   auto itr = vec.begin();
   for ( size_t n = 0; n < val ; ++itr, ++hTItr, ++n )
      {
      ASSERT_TRUE( CompareEqual( (*itr)->tuple.key, hTItr->key ) );
      ASSERT_EQUAL( (*itr)->tuple.value, hTItr->value );
      }
   ASSERT_TRUE( itr == vec.end() && hTItr == hashTable.end() );
   }

TEST ( test_topN )
   {
   static size_t val = 10;
   HashTable<const char*, size_t> hashTable(1); // purposely absolute worst case for insertion sort
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   for ( unsigned n = 0; n < val; ++n )
      {
      strings.emplace_back( std::to_string( n ) );
      const char * ptr = strings[n].c_str();
      hashTable.Find( ptr, n );
      Tuple<const char*, size_t> * kv = hashTable.Find( strings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->value, n );
      } // end for
   
   ASSERT_EQUAL( hashTable.getCollisions(), val - 1 );
   
   Tuple< const char *, size_t > **top10 = TopN( &hashTable, 15 );
   Tuple< const char *, size_t > *p;
   int i = 0;
   for ( size_t n = val - 1;  i < 15 && ( p = top10[ i ] );  i++, --n )
      ASSERT_EQUAL( n, p->value );

   ASSERT_EQUAL( (size_t)i , val );

   delete [ ] top10;

   hashTable.Optimize();
   ASSERT_EQUAL( hashTable.getCollisions(), hashTable.size() - hashTable.numOfLinkedLists() );
   top10 = TopN( &hashTable, 15 );

   i = 0;
   for ( size_t n = val - 1;  i < 15 && ( p = top10[ i ] );  i++, --n )
      ASSERT_EQUAL( n, p->value );

   ASSERT_EQUAL( (size_t)i , val );

   delete [ ] top10;

   top10 = TopN( &hashTable, 5 );

   i = 0;
   for ( size_t n = val - 1;  i < 5 && ( p = top10[ i ] );  i++, --n )
      ASSERT_EQUAL( n, p->value );

   ASSERT_EQUAL( i , 5 );

   delete [ ] top10;
   }

TEST_MAIN()