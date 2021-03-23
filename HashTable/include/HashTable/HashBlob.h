#pragma once

// HashBlob, a serialization of a HashTable into one contiguous
// block of memory, possibly memory-mapped to a HashFile.

// Nicole Hamilton  nham@umich.edu

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
#ifdef MACOS
   #include <malloc/malloc.h>
#endif
#ifdef LINUX
   #include <malloc.h>
#endif
#include <unistd.h>
#include <sys/mman.h>

#include "HashTable.h"


using Hash = HashTable< const char *, size_t >;
using Pair = Tuple< const char *, size_t >;
using HashBucket = Bucket< const char *, size_t >;


static const size_t Unknown = 0;


size_t RoundUp( size_t length, size_t boundary = 8 )
   {
   // Round up to the next multiple of the boundary, which
   // must be a power of 2.

   static const size_t oneless = boundary - 1,
      mask = ~( oneless );
   return ( length + oneless ) & mask;
   }


struct SerialTuple
   {
   // This is a serialization of a HashTable< char *, size_t >::Bucket.
   // One is packed up against the next in a HashBlob.

   // Since this struct includes size_t and uint32_t members, we'll
   // require that it be sizeof( size_t ) aligned to avoid unaligned
   // accesses.

   public:

      // SerialTupleLength = 0 is a sentinel indicating
      // this is the last SerialTuple chained in this list.
      // (Actual length is not given but not needed.)

      size_t Length, Value;
      uint32_t HashValue;

      // The Key will be a C-string of whatever length.
      char Key[ Unknown ];

      // Calculate the bytes required to encode a HashBucket as a
      // SerialTuple.

      static size_t helperBytesRequired( const Pair& pair )
         {
         size_t sizeOfMetaData = sizeof( size_t ) * 2 + sizeof( uint32_t );

         size_t keyLen = strlen( pair.key );

         return RoundUp( sizeOfMetaData + keyLen, 0x8 );
         }

      static size_t BytesRequired( const HashBucket *b )
         {
         // Your code here.
         return helperBytesRequired( b->tuple );
         }

      // Write the HashBucket out as a SerialTuple in the buffer,
      // returning a pointer to one past the last character written.

      static char *Write( char *buffer, char *bufferEnd,
            const HashBucket *b )
         {
         // Your code here.

         return nullptr;
         }
  };


class HashBlob
   {
   // This will be a hash specifically designed to hold an
   // entire hash table as a single contiguous blob of bytes.
   // Pointers are disallowed so that the blob can be
   // relocated to anywhere in memory

   // The basic structure should consist of some header
   // information including the number of buckets and other
   // details followed by a concatenated list of all the
   // individual lists of tuples within each bucket.

   public:

      // Define a MagicNumber and Version so you can validate
      // a HashBlob really is one of your HashBlobs.

      size_t MagicNumber,
         Version,
         BlobSize,
         NumberOfBuckets,
         Buckets[ Unknown ];

      // The SerialTuples will follow immediately after.

      template<class HashFunc = FNV> 
      const SerialTuple *Find( const char *key ) const
         {
         static HashFunc func;
         static constexpr size_t offsetToBuckets = sizeof( size_t ) * 4;
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not found,
         // return nullptr.

         uint32_t hashVal = func( key );
         size_t tupleInd = hashVal & ( NumberOfBuckets - 1 );

         if ( tupleInd )
            {
            SerialTuple const *tupleArr = reinterpret_cast<SerialTuple const *>( &Buckets + tupleInd - offsetToBuckets );
            for ( ; tupleArr->Length; ++tupleArr )
               if ( CompareEqual( tupleArr->Key, key ) )
                  return tupleArr;
            } // end if
         return nullptr;
         }

      inline static size_t BytesForHeaderBuckets( const Hash *hashTable )
         {
         size_t sizeOfBuckets = sizeof( size_t ) * hashTable->tableSize;
         size_t header = sizeof( HashBlob );
         return sizeOfBuckets + header;
         }

      static size_t BytesRequired( const Hash *hashTable )
         {
         // Calculate how much space it will take to
         // represent a HashTable as a HashBlob.

         // Need space for the header + buckets +
         // all the serialized tuples.

         // Your code here.
         size_t totSizeOfSerialTuples = 0; // Count bytes that go to serial tuples...

         // Add up the 
         for ( Hash::const_iterator itr = hashTable->cbegin(); itr != hashTable->cend(); ++itr )
            totSizeOfSerialTuples += SerialTuple::helperBytesRequired( *itr );

         // Add null sentinal bytes
         totSizeOfSerialTuples += hashTable->numOfLinkedLists() * sizeof( SerialTuple );

         size_t totBytes = totSizeOfSerialTuples + BytesForHeaderBuckets( hashTable );
         // It should be the case that the resultant value is a multiple of 8
         assert( RoundUp( totBytes, 8 ) == totBytes );
         return totBytes;
         }

      // Write a HashBlob into a buffer, returning a
      // pointer to the blob.

      static HashBlob *Write( HashBlob *hb, size_t bytes,
            const Hash *hashTable )
         {
         // Your code here.

         return nullptr;
         }

      // Create allocates memory for a HashBlob of required size
      // and then converts the HashTable into a HashBlob.
      // Caller is responsible for discarding when done.

      // (No easy way to override the new operator to create a
      // variable sized object.)

      static HashBlob *Create( const Hash *hashTable )
         {
         // Your code here.

         return nullptr;
         }

      // Discard

      static void Discard( HashBlob *blob )
         {
         // Your code here.
         }
   };

class HashFile
   {
   private:

      HashBlob *blob;

      size_t FileSize( int f )
         {
         struct stat fileInfo;
         fstat( f, &fileInfo );
         return fileInfo.st_size;
         }

   public:

      const HashBlob *Blob( )
         {
         return blob;
         }

      HashFile( const char *filename )
         {
         // Open the file for reading, map it, check the header,
         // and note the blob address.

         // Your code here.
         }

      HashFile( const char *filename, const Hash *hashtable )
         {
         // Open the file for write, map it, write
         // the hashtable out as a HashBlob, and note
         // the blob address.

         // Your code here.
         }

      ~HashFile( )
         {
         // Your code here.
         }
   };
