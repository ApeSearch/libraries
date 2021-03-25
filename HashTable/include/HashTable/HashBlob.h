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

constexpr size_t RoundUpConstExpr( size_t length, size_t boundary = 8 )
   {
   // Round up to the next multiple of the boundary, which
   // must be a power of 2.

   const size_t oneless = boundary - 1,
      mask = ~( oneless );
   return ( length + oneless ) & mask;
   }

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

      // Should be 20
      static constexpr size_t sizeOfMetaData = sizeof( size_t ) * 2 + sizeof( uint32_t );
      static constexpr size_t sizeOfNullSentinel = sizeof( size_t ); // Just needs to be a Length

      // The Key will be a C-string of whatever length.
      char Key[ Unknown ];

      // Calculate the bytes required to encode a HashBucket as a
      // SerialTuple.

      static size_t helperBytesRequired( const Pair& pair )
         {
         // Extra character for null-character
         size_t keyLen = strlen( pair.key ) + 1;

         return RoundUp( SerialTuple::sizeOfMetaData + keyLen, 0x8 );
         }

      static size_t BytesRequired( const HashBucket *b )
         {
         return helperBytesRequired( b->tuple );
         }

      // Write the HashBucket out as a SerialTuple in the buffer,
      // returning a pointer to one past the last character written.

      static SerialTuple *initSerialTuple( char *buffer, const HashBucket *b, size_t lengthSerialized )
         {
         SerialTuple *serialTuple = reinterpret_cast< SerialTuple * > ( buffer );

         serialTuple->Length = lengthSerialized;
         serialTuple->Value = b->tuple.value;
         serialTuple->HashValue = b->hashValue;

         return serialTuple;
         }

      static char *Write( char *buffer, char *bufferEnd,
            const HashBucket *b )
         {
         // Your code here.
         size_t bytesReq = BytesRequired( b );

         assert( bufferEnd - buffer >= bytesReq );

         SerialTuple *serialTuple = initSerialTuple( buffer, b, bytesReq );

         char *key = serialTuple->Key;

         size_t charToWrite = bytesReq - SerialTuple::sizeOfMetaData;
         assert( key + charToWrite <= bufferEnd );

         assert( strncpy( key, b->tuple.key, charToWrite ) == key );

         return buffer + bytesReq;
         } // end Write()
      
      static char *WriteNull( char *buffer, char *bufferEnd )
         {
         // Ensures that the buffer can fit the null sentinel to begin with...
         assert( size_t( bufferEnd - buffer ) >= SerialTuple::sizeOfNullSentinel );

         SerialTuple *nullSerial = reinterpret_cast< SerialTuple * >( buffer ); 
         nullSerial->Length = 0;

         return buffer + SerialTuple::sizeOfNullSentinel;
         // not buffer + SerialTuple::sizeOfMetaData since memory is not aligned 8-byte
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
   friend class Const_Iterator;

   public:
      static constexpr size_t decidedMagicNum = 69;
      static constexpr size_t verison = 1;

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

         uint8_t const *byteAddr = reinterpret_cast< uint8_t const * > ( &MagicNumber );

         uint32_t hashVal = ( uint32_t )func( key );
         size_t bucketInd = hashVal & ( NumberOfBuckets - 1 );
         size_t offset = Buckets[ bucketInd ];

         if ( offset )
            {
            // Okay since everything is continous in memory...
            byteAddr += offset;
            SerialTuple const *tupleArr = reinterpret_cast<SerialTuple const *>( byteAddr );

            while ( tupleArr->Length )
               {
               if ( CompareEqual( tupleArr->Key, key ) )
                  return tupleArr;
               byteAddr += tupleArr->Length;
               tupleArr = reinterpret_cast<SerialTuple const *>( byteAddr );
               } // end if
            }
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
         totSizeOfSerialTuples += hashTable->numOfLinkedLists() * SerialTuple::sizeOfNullSentinel;

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

         // placeholders for now
         hb->MagicNumber = HashBlob::decidedMagicNum;
         hb->Version = HashBlob::verison;

         
         hb->BlobSize = bytes;
         hb->NumberOfBuckets = hashTable->table_size();

         // Points to the beginning of the Serial Tuples...
         char *serialPtr =reinterpret_cast< char *>( hb->Buckets + hashTable->table_size() );
         char *end = reinterpret_cast< char *>( hb ) + bytes;

         std::vector< std::vector< HashBucket *> > buckets( hashTable->vectorOfBuckets() );

         typename std::vector< std::vector< HashBucket *> >::iterator bucketsVec = buckets.begin();

         for ( ; bucketsVec != buckets.end(); ++bucketsVec )
            {
            // Write the absolute address into the bucket represented...
            size_t bucketInd = bucketsVec->front()->hashValue & ( hashTable->table_size() - 1 );
            hb->Buckets[ bucketInd ] =  serialPtr - reinterpret_cast< char *>( hb );  

            for ( typename std::vector<HashBucket*>::iterator bucket = bucketsVec->begin(); 
               bucket != bucketsVec->end(); ++bucket )
               {
               serialPtr = SerialTuple::Write( serialPtr, end, *bucket );
               } // end for
            
            
            // Now add null serial Tuple
            serialPtr = SerialTuple::WriteNull( serialPtr, end );
            } // end for
         assert( end == serialPtr );
         return hb;
         }

      // Create allocates memory for a HashBlob of required size
      // and then converts the HashTable into a HashBlob.
      // Caller is responsible for discarding when done.

      // (No easy way to override the new operator to create a
      // variable sized object.)
      static HashBlob *Create( const Hash *hashTable )
         {
         // Your code here.
         const size_t bytesReq = HashBlob::BytesRequired( hashTable );

         // Need to use malloc to get the exact right number of bytes
         char *buffer = ( char * ) malloc( bytesReq );
         memset( buffer, 0, bytesReq );

         return Write( reinterpret_cast<HashBlob *>( buffer ), bytesReq, hashTable );
         }

      // Discard
      static void Discard( HashBlob *blob )
         {
         free( blob );
         }

      class Const_Iterator
         {
         
         friend class HashBlob;

         HashBlob const *hashBlob;

         char const *buffer;
         char const * const bufferEnd;

         inline void advanceSerialTuple( )
            {
            assert( buffer != bufferEnd );

            SerialTuple const *tuple = reinterpret_cast< SerialTuple const * >( buffer );

            // Needs to point to a valid serial tuple ( not null )
            assert( tuple->Length );

            buffer += tuple->Length;

            tuple = reinterpret_cast< SerialTuple const * >( buffer );

            // check if reached null serial; if so, advance again.
            if ( buffer != bufferEnd && !tuple->Length )
               buffer += SerialTuple::sizeOfNullSentinel;

            } // end advanceSerialTuple()

         Const_Iterator( HashBlob const *hb, char const *end, bool start ) : hashBlob( hb ), 
            buffer( start && hb ? reinterpret_cast< char const * >( hb->Buckets + hb->NumberOfBuckets ) : end ), 
            bufferEnd( end ) {}

      public:
         Const_Iterator( ) : hashBlob( nullptr ), buffer( nullptr ), bufferEnd( nullptr ) {}


         ~Const_Iterator( ) {}

         const SerialTuple &operator*( ) const
            {
            return *( reinterpret_cast< SerialTuple const *> ( buffer ) );
            } // end Dereference operator()

         SerialTuple const *operator->( ) const
            {
            return reinterpret_cast< SerialTuple const *>( buffer );
            }

         Const_Iterator &operator++( )
            {
            advanceSerialTuple();
            return *this;
            } // end prefix

         Const_Iterator operator++(int)
            {
            Const_Iterator old( *this );
            advanceSerialTuple();
            return old;
            } // end postfix

         bool operator==( const Const_Iterator & rhs ) const
            {
            return buffer == rhs.buffer;
            }
         bool operator!=( const Const_Iterator & rhs ) const
            {
            return buffer != rhs.buffer;
            }

         Const_Iterator operator+( ssize_t var )
            {
            Const_Iterator copy( *this );
            for ( ssize_t n = 0; n < var; ++n )
               copy.advanceSerialTuple();
            return copy;
            } // end operator+()
         };
      
      Const_Iterator cbegin( char const *end ) const
         {
         return Const_Iterator( this, end, true );
         }

      Const_Iterator cend( char const *end ) const
         {
         return Const_Iterator( this, end, false );
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
         HashBlob::Discard( blob );
         }
   };
