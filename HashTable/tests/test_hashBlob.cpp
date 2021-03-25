#include "../include/HashTable/HashTable.h"
#include "../include/HashTable/HashBlob.h"
#include "../../unit_test_framework/include/unit_test_framework/unit_test_framework.h"
#include <sys/mman.h>

static void testingFind( HashTable<const char*, size_t>& hashTable );

TEST( test_BytesForHeaderBuckets )
   {
    
    HashTable<const char*, size_t> hashTable(8);
    testingFind( hashTable ); // Insert into hashTable...

    size_t bytesReq = HashBlob::BytesForHeaderBuckets( &hashTable ); 
    size_t expectedBytes = ( 4 + hashTable.table_size() ) * sizeof( size_t ); // 4 * 8ish
    ASSERT_EQUAL( bytesReq, expectedBytes );
    
   }

TEST( test_helperBytesRequire )
   {
   HashTable<const char*, size_t> hashTable(8);
   testingFind( hashTable ); // Insert into hashTable...


   size_t bytesReq = HashBlob::BytesForHeaderBuckets( &hashTable ); 
   size_t expectedBytes = ( 4 + hashTable.table_size() ) * sizeof( size_t ); // 4 * 8ish
   ASSERT_EQUAL( bytesReq, expectedBytes );
   

   Hash::const_iterator itr = hashTable.cbegin();
   size_t serialTupleSize = sizeof( size_t ) * 2 + sizeof( uint32_t );

   for ( Hash::const_iterator itr = hashTable.cbegin(); itr != hashTable.cend(); ++itr )
      {
      if ( CompareEqual( itr->key, "testing" ) || CompareEqual( itr->key, "lololol" ) )
         {
         ASSERT_EQUAL( SerialTuple::helperBytesRequired( *itr ), 32 );
         }
      else if ( CompareEqual( itr->key, "1 2 3 3 4 5 " ) )
         {
         ASSERT_EQUAL( strlen( "1 2 3 3 4 5 " ), 12 );
         ASSERT_EQUAL( SerialTuple::helperBytesRequired( *itr ), 40 );
         }
      } // end for
   }

TEST( test_helperBytesRequireStrict )
   {
   HashTable<const char*, size_t> hashTable(8);

    hashTable.Find( "testing", 100 );
    hashTable.Find( "lololol", 32 );
    hashTable.Find( "1 2 3 3 4 5", 90 );

   size_t bytesReq = HashBlob::BytesForHeaderBuckets( &hashTable ); 
   size_t expectedBytes = ( 4 + hashTable.table_size() ) * sizeof( size_t ); // 4 * 8ish
   ASSERT_EQUAL( bytesReq, expectedBytes );

   Hash::const_iterator itr = hashTable.cbegin();
   size_t serialTupleSize = sizeof( size_t ) * 2 + sizeof( uint32_t );

   for ( Hash::const_iterator itr = hashTable.cbegin(); itr != hashTable.cend(); ++itr )
      {
      if ( CompareEqual( itr->key, "testing" ) || CompareEqual( itr->key, "lololol" ) )
         {
         ASSERT_EQUAL( SerialTuple::helperBytesRequired( *itr ), 32 );
         }
      else if ( CompareEqual( itr->key, "1 2 3 3 4 5 " ) )
         {
         ASSERT_EQUAL( strlen( "1 2 3 3 4 5" ) + 1, 12 );
         ASSERT_EQUAL( SerialTuple::helperBytesRequired( *itr ), 32 );
         }
      } // end for
   }

TEST( test_TotBytesRequired )
   {
   HashTable<const char*, size_t> hashTable(8);
   testingFind( hashTable ); // Insert into hashTable...

   size_t headerAndBucketsBytes = HashBlob::BytesForHeaderBuckets( &hashTable ); 
   size_t charactersTaken = strlen( "testing" ) + strlen( "lololol") + strlen( "1 2 3 3 4 5 " );


    // "testing" == 32
    // "lololol" == 32
    // "1 2 3 3 4 5 " == 40

   ASSERT_EQUAL( hashTable.size(), 3 );
   ASSERT_EQUAL( hashTable.numOfLinkedLists(), 3 );

   // 104 are from the valid serial tuples while 24 ( 8 * 3 ) the null serial tuples
   ASSERT_EQUAL( HashBlob::BytesRequired(( &hashTable )), 104 + 
      ( hashTable.numOfLinkedLists() * SerialTuple::sizeOfNullSentinel ) + headerAndBucketsBytes );
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

TEST( test_SerialTupleWrite )
   {
   const char *key = "testing";
   const size_t value = 69;
   const uint32_t hashValue = 1000009;
   Bucket< const char *, size_t > bucket( key, value, hashValue );

   constexpr static size_t bytesReq = RoundUpConstExpr( SerialTuple::sizeOfMetaData + 7 );

   char buffer[ bytesReq ];

   char *end = SerialTuple::Write( buffer, buffer + bytesReq, &bucket );

   SerialTuple *serialTuple = reinterpret_cast< SerialTuple *>( &buffer );

   ASSERT_EQUAL( buffer + bytesReq, end );
   ASSERT_EQUAL( serialTuple->Length, bytesReq );
   ASSERT_EQUAL( serialTuple->Value, value );
   ASSERT_EQUAL( serialTuple->HashValue, hashValue );
   ASSERT_TRUE( CompareEqual( serialTuple->Key, key ) );
   }

TEST( test_SerialTupleWriteStrict )
   {
   const char *key = "test";
   const size_t value = 69;
   const uint32_t hashValue = 1000009;
   Bucket< const char *, size_t > bucket( key, value, hashValue );

   constexpr static size_t bytesReq = RoundUpConstExpr( SerialTuple::sizeOfMetaData + 5 );

   char buffer[ bytesReq ];

   char *end = SerialTuple::Write( buffer, buffer + bytesReq, &bucket );

   SerialTuple *serialTuple = reinterpret_cast< SerialTuple *>( &buffer );

   ASSERT_EQUAL( buffer + bytesReq, end );
   ASSERT_EQUAL( RoundUp( end - buffer, 8 ), bytesReq );
   ASSERT_EQUAL( serialTuple->Length, bytesReq );
   ASSERT_EQUAL( serialTuple->Value, value );
   ASSERT_EQUAL( serialTuple->HashValue, hashValue );
   ASSERT_TRUE( CompareEqual( serialTuple->Key, key ) );
   }

#define FILEPATH "./tests/hashFiles/test_hashBlob.txt"


TEST( test_SerialTupleWrite_WITHFILEWRITE )
   {
   const char *key = "testing";
   const size_t value = 69;
   const uint32_t hashValue = 1000009;
   Bucket< const char *, size_t > bucket( key, value, hashValue );

   constexpr static size_t bytesReq = RoundUpConstExpr( SerialTuple::sizeOfMetaData + 7 );

   int fd;
   //char *filename =  "./tests/hashFiles/test_hashBlob.txt";
   
   // O_RDWR == open for reading and writing
   // O_CREAT == create file if not alrady exits
   fd = open( FILEPATH, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600 ); // 0600 == 0400 | 0200
   if ( fd == -1 )
      {
      perror("Error opening file");
      ASSERT_TRUE( false );
      }

   int result = lseek( fd, bytesReq - 1, SEEK_SET );
   if ( result == -1 )
      {
      close( fd );
      perror( "Error calling lseek() to stretch file" );
      ASSERT_TRUE( false );
      } // end if

   result = write( fd, "", 1 );
   if ( result != 1 )
      {
      close( fd );
      perror( "Error writing byte to file " );
      ASSERT_TRUE( false );
      } // end if


   void *map;
   map = mmap( 0, bytesReq, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
   if ( map == MAP_FAILED )
      {
      close( fd );
      perror( "Error mmapping file" );
      ASSERT_TRUE( false );
      }
   
   char *buffer = reinterpret_cast< char * >( map );

   char *end = SerialTuple::Write( buffer, buffer + bytesReq, &bucket );

   SerialTuple *serialTuple = reinterpret_cast< SerialTuple *>( buffer );

   ASSERT_EQUAL( buffer + bytesReq, end );
   ASSERT_EQUAL( serialTuple->Length, bytesReq );
   ASSERT_EQUAL( serialTuple->Value, value );
   ASSERT_EQUAL( serialTuple->HashValue, hashValue );
   ASSERT_TRUE( CompareEqual( serialTuple->Key, key ) );

   // Free mmapped memory 
   if ( munmap( map, bytesReq ) == -1 )
      {
      perror( "err un-mapping the file " );
      close( fd );
      ASSERT_TRUE( false );
      }


   close( fd );
   }

TEST( test_SerialTupleWrite_WITHFILEWRITE_READAFTER )
   {
   const char *key = "testing";
   const size_t value = 69;
   const uint32_t hashValue = 1000009;
   constexpr static size_t bytesReq = RoundUpConstExpr( SerialTuple::sizeOfMetaData + 7 );

   int fd;
   void *map;

   fd = open( FILEPATH, O_RDONLY );
   if ( fd == -1 )
      {
      perror( "Error opening file for reading" );
      ASSERT_TRUE( false );
      }
   
   map = mmap( 0, bytesReq, PROT_READ, MAP_SHARED, fd, 0 );
   if ( map == MAP_FAILED )
      {
      close( fd );
      perror( "Error mmapping the file" );
      ASSERT_TRUE( false );
      } // end if

   SerialTuple *serialTuple = reinterpret_cast< SerialTuple *>( map );

   ASSERT_EQUAL( serialTuple->Length, bytesReq );
   ASSERT_EQUAL( serialTuple->Value, value );
   ASSERT_EQUAL( serialTuple->HashValue, hashValue );
   ASSERT_TRUE( CompareEqual( serialTuple->Key, key ) );

    // Free mmapped memory 
   if ( munmap( map, bytesReq ) == -1 )
      {
      perror( "err un-mapping the file " );
      close( fd );
      ASSERT_TRUE( false );
      }
   close( fd );
   }

TEST( test_hashBlob_basic )
   {
   HashTable<const char*, size_t> hashTable(1);
   hashTable.Find( "testing", 100 );
   hashTable.Find( "lololol", 101 );
   hashTable.Find( "1 2 3 3 4 5", 102 );

   // Allocate resources
   HashBlob *hb = HashBlob::Create( &hashTable );

   const SerialTuple *tuple = hb->Find( "testing" );
   ASSERT_EQUAL( tuple->Value, 100 );
   ASSERT_TRUE(  CompareEqual( tuple->Key, "testing" ) );

   tuple = hb->Find( "lololol" );
   ASSERT_EQUAL( tuple->Value, 101 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "lololol" ) );

   tuple = hb->Find( "1 2 3 3 4 5" );
   ASSERT_EQUAL( tuple->Value, 102 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "1 2 3 3 4 5" ) );

   tuple = hb->Find( "Nope Aint finding this sht ");
   ASSERT_TRUE( !tuple );

   // Free resources
   HashBlob::Discard( hb );
   }


TEST( test_hashBlob_basicWItr )
   {
   HashTable<const char*, size_t> hashTable(1);
   hashTable.Find( "testing", 100 );
   hashTable.Find( "lololol", 101 );
   hashTable.Find( "1 2 3 3 4 5", 102 );

   // Allocate resources

   size_t bytesReq = HashBlob::BytesRequired( &hashTable );

   uint8_t hbStack[ bytesReq ];

   HashBlob *hb = HashBlob::Write( reinterpret_cast< HashBlob *>( hbStack ), bytesReq,
      &hashTable );

   char const *end = reinterpret_cast< char *>( hb ) + HashBlob::BytesRequired( &hashTable );
   ASSERT_EQUAL( reinterpret_cast< char const *>( hbStack + bytesReq ), end );

   HashBlob::Const_Iterator constItr = hb->cbegin( end );

   const SerialTuple *tuple = hb->Find( "testing" );
   ASSERT_EQUAL( tuple->Value, 100 );
   ASSERT_TRUE(  CompareEqual( tuple->Key, "testing" ) );

   ASSERT_EQUAL( constItr->Value, 100 );
   ASSERT_TRUE( CompareEqual( constItr->Key, "testing" ) );
   ++constItr;

   tuple = hb->Find( "lololol" );
   ASSERT_EQUAL( tuple->Value, 101 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "lololol" ) );

   ASSERT_EQUAL( constItr->Value, 101 );
   ASSERT_TRUE( CompareEqual( constItr->Key, "lololol" ) );
   ++constItr;

   tuple = hb->Find( "1 2 3 3 4 5" );
   ASSERT_EQUAL( tuple->Value, 102 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "1 2 3 3 4 5" ) );

   ASSERT_EQUAL( constItr->Value, 102 );
   ASSERT_TRUE( CompareEqual( constItr->Key, "1 2 3 3 4 5" ) );
   ++constItr;

   ASSERT_EQUAL( constItr, hb->cend( end ) );

   }

TEST( test_hashBlob_basic_Opt )
   {
   HashTable<const char*, size_t> hashTable(1);
   hashTable.Find( "testing", 100 );
   hashTable.Find( "lololol", 101 );
   hashTable.Find( "1 2 3 3 4 5", 102 );

   hashTable.Optimize();

   // Allocate resources
   HashBlob *hb = HashBlob::Create( &hashTable );

   const SerialTuple *tuple = hb->Find( "testing" );
   ASSERT_EQUAL( tuple->Value, 100 );
   ASSERT_TRUE(  CompareEqual( tuple->Key, "testing" ) );

   tuple = hb->Find( "lololol" );
   ASSERT_EQUAL( tuple->Value, 101 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "lololol" ) );

   tuple = hb->Find( "1 2 3 3 4 5" );
   ASSERT_EQUAL( tuple->Value, 102 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "1 2 3 3 4 5" ) );

   tuple = hb->Find( "Nope Aint finding this sht ");
   ASSERT_TRUE( !tuple );

   // Free resources
   HashBlob::Discard( hb );
   }


TEST( test_hashBlob_basicWItr_Opt )
   {
   HashTable<const char*, size_t> hashTable(1);
   hashTable.Find( "testing", 100 );
   hashTable.Find( "lololol", 101 );
   hashTable.Find( "1 2 3 3 4 5", 102 );

   hashTable.Optimize();

   // Allocate resources

   size_t bytesReq = HashBlob::BytesRequired( &hashTable );

   uint8_t *hbStack = new uint8_t[ bytesReq ];

   HashBlob *hb = HashBlob::Write( reinterpret_cast< HashBlob *>( hbStack ), bytesReq,
      &hashTable );

   char const *end = reinterpret_cast< char *>( hb ) + HashBlob::BytesRequired( &hashTable );
   ASSERT_EQUAL( reinterpret_cast< char const *>( hbStack + bytesReq ), end );

   HashBlob::Const_Iterator constItr = hb->cbegin( end );

   const SerialTuple *tuple = hb->Find( "testing" );
   ASSERT_EQUAL( tuple->Value, 100 );
   ASSERT_TRUE(  CompareEqual( tuple->Key, "testing" ) );

   tuple = hb->Find( "lololol" );
   ASSERT_EQUAL( tuple->Value, 101 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "lololol" ) );

   tuple = hb->Find( "1 2 3 3 4 5" );
   ASSERT_EQUAL( tuple->Value, 102 );
   ASSERT_TRUE( CompareEqual( tuple->Key, "1 2 3 3 4 5" ) );

   for ( ; constItr != hb->cend( end ); ++constItr )
      {
      if ( CompareEqual( constItr->Key, "testing" ) )
         {
         ASSERT_EQUAL( constItr->Value, 100 );
         }
      else if ( CompareEqual( tuple->Key, "lololol" ) )
         {
         ASSERT_EQUAL( constItr->Value, 101 );
         }
      else if ( CompareEqual( tuple->Key, "1 2 3 3 4 5" ) )
         {
         ASSERT_EQUAL( tuple->Value, 102 );
         }
      else // Must be one of these values
         {
         ASSERT_TRUE( false );
         }
      } // end for
   ASSERT_EQUAL( constItr, hb->cend( end ) );

   delete []hbStack;
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

   for ( int n = val - 1, ind = 0; n >= 0; --n, ++ind )
      {
      std::string str = std::to_string( n );
      const Tuple<const char*, size_t>& tuple = vec[(size_t) ind]->tuple;
      ASSERT_EQUAL( tuple.key, std::to_string( n ) );
      ASSERT_EQUAL( tuple.value , n );
      } // end for
   }

TEST( test_hashBlobExtensive )
   {
    static size_t val = 10000;
   HashTable<const char*, size_t> hashTable;
   HashBlob *hb;
   std::vector<std::string> copyOfStrings;
   {
   std::vector<std::string> strings; // To keep pointers around
   strings.reserve( val ); // Very important
   testingFlattening( strings, val, hashTable );
   copyOfStrings = strings; // Copy every string ( same but different addresses )

   ASSERT_EQUAL( strings.size(), hashTable.size() );
   ASSERT_EQUAL( 4096, hashTable.table_size() );
   hashTable.Optimize(); // Current load factor becomes at most 0.5
   ASSERT_EQUAL( strings.size(), hashTable.size() );
   hb = HashBlob::Create( &hashTable );
   } // Delete memory of original strings to show that hb will not seg fault

   for ( int n = val - 1; n >= 0; --n )
      {
      std::string copy( copyOfStrings[n] );
      const SerialTuple *kv = hb->Find( copyOfStrings[n].c_str() );
      ASSERT_TRUE( kv );
      ASSERT_EQUAL( kv->Value, n );
      ASSERT_TRUE( CompareEqual( kv->Key, copyOfStrings[n].c_str() ) );

      } // end for

   HashBlob::Discard( hb );
   }

TEST_MAIN()