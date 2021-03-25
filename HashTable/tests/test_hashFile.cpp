#include "../include/HashTable/HashTable.h"
#include "../include/HashTable/HashBlob.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"
#include <sys/mman.h>
#include <utility>

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

TEST( test_hashFile )
    {
    char const *filename = "./tests/hashFiles/hashFile.txt";

    static size_t val = 10000;
    HashTable<const char*, size_t> hashTable;
    std::vector<std::string> strings; // To keep pointers around
    strings.reserve( val ); // Very important
    testingFlattening( strings, val, hashTable );

    ASSERT_EQUAL( strings.size(), hashTable.size() );
    ASSERT_EQUAL( 4096ul, hashTable.table_size() );
    hashTable.Optimize(); // Current load factor becomes at most 0.5
    ASSERT_EQUAL( strings.size(), hashTable.size() );

    HashFile hashFile( filename, &hashTable );

    HashBlob const *hb = hashFile.Blob();

    for ( int n = (int)val - 1; n >= 0; --n )
        {
        const SerialTuple *kv = hb->Find( strings[(size_t)n].c_str() );
        ASSERT_TRUE( kv );
        ASSERT_EQUAL( kv->Value, (size_t)n );
        ASSERT_TRUE( CompareEqual( kv->Key, strings[(size_t)n].c_str() ) );
        } // end for
    } 

TEST( test_hashFileReadAfter )
    {
    char const *filename = "./tests/hashFiles/hashFile.txt";

    static size_t val = 10000;
    std::vector<std::string> strings; // To keep pointers around
    strings.reserve( val ); // Very important
    for ( unsigned n = 0; n < val; ++n )
      strings.emplace_back( std::to_string( n ) );

    HashFile hashFile( filename );

    HashBlob const *hb = hashFile.Blob();

    for ( int n = (int)val - 1; n >= 0; --n )
        {
        const SerialTuple *kv = hb->Find( strings[(size_t)n].c_str() );
        ASSERT_TRUE( kv );
        ASSERT_EQUAL( kv->Value, (size_t)n );
        ASSERT_TRUE( CompareEqual( kv->Key, strings[(size_t)n].c_str() ) );
        } // end for
    } 


TEST_MAIN()