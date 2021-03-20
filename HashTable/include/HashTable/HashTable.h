// Simple hash table template.

#pragma once

#include <cassert>
#include <cstdint>
#include <cstring> // for strlen
#include <assert.h>
#include <algorithm> // for std::sort
#include <vector>
using std::sort;
#include "../../../AS/include/AS/algorithms.h"

#define DEFAULTSIZE 4096

static inline size_t computeTwosPowCeiling( ssize_t num ) 
   {
   --num; // Account for num already being a two's power
   size_t powerNum = 1;
   for (; num > 0; num >>=1 )
      powerNum <<= 1;
   return powerNum;
   }

class FNV
{
public:
   FNV() = default;
   size_t operator()( const char *data ) const
      {
      //TODO optimize 
      size_t length = strlen(data);

      static const size_t FnvOffsetBasis=146959810393466560;
      static const size_t FnvPrime=1099511628211ul;
      size_t hash = FnvOffsetBasis;
      for (  size_t i = 0; i < length; ++i)
         {
         hash *= FnvPrime;
         hash ^= (unsigned long)data[ i ];
         } // end for
      return hash;
      } //end operator()
};

// Compare C-strings, return true if they are the same.
bool CompareEqual( const char *L, const char *R );


template< typename Key, typename Value > class Tuple
   {
   public:
      Key key;
      Value value;

      Tuple( const Key &k, const Value v ) : key( k ), value( v )
         {
         }
   };


template< typename Key, typename Value > class Bucket
   {
   public:
      Bucket *next;
      uint32_t hashValue;
      Tuple< Key, Value > tuple;

      Bucket( const Key &k, const Value v, const uint32_t h ) :
            tuple( k, v ), next( nullptr ) , hashValue( h )
         {
         }
      ~Bucket()
         {
         //Recursive delete on bucket-chain
         delete next;
         } // end ~Bucket()
  };

template< typename Key, typename Value, class Hash = FNV > class HashTable
   {
   private:

      // Your code here.

      Bucket< Key, Value > **buckets;
      size_t tableSize; // length of bucket array
      size_t numberOfBuckets; // Contains amount of seperate chained buckets
      size_t collisions; // Tracks current collisions in hash_table
      Hash hashFunc;

      friend class Iterator;
      friend class HashBlob;
   
   std::vector< Bucket< Key, Value> *> flattenHashTable()
      {
      
      Bucket< Key, Value > *currBucket = *buckets;
      Bucket< Key, Value > **mainLevel = buckets;
      std::vector< Bucket< Key, Value> * > bucketVec;
      bucketVec.reserve( numberOfBuckets );

      // ++ happens first then dereference
      for ( Bucket< Key, Value > **const end = buckets + tableSize; 
            mainLevel != end && bucketVec.size() < numberOfBuckets; ++mainLevel )
         {
         for ( currBucket = *mainLevel ; currBucket ; currBucket = currBucket->next )
            bucketVec.emplace_back( currBucket );
         }

      return bucketVec;
      }  // end flattenHashTable()


   Bucket< Key, Value > **helperFind( const Key& k, uint32_t hashVal ) const
      {
      // Applying a bit-wise mask on the least-sig bits
      uint32_t index = hashVal & ( tableSize - 1 ); // modulo operation using bitwise AND (only works with power of two table size)
      Bucket< Key, Value > **bucket = buckets + index;

      for ( ; *bucket; bucket = &( *bucket )->next )
         {
         //if( ( *bucket )->tuple.key == k )
         if ( CompareEqual( ( *bucket )->tuple.key, k ) )
            break;
         }
      return bucket;
      } // end helperFind()

   public:

      Tuple< Key, Value > *Find( const Key& k, const Value initialValue )
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not already
         // in the hash, add it with the initial value.

         // Your code here
         uint32_t hashVal = hashFunc( k );
         Bucket< Key, Value > **bucket = helperFind( k, hashVal );
         
         // Checks for nullptr
         if( !*bucket )
            {
            *bucket = new Bucket< Key, Value >( k, initialValue, hashVal );
            ++numberOfBuckets;
            if( bucket < buckets || bucket >= buckets + tableSize ) 
               ++collisions;
            } // end if
      
         return & ( * bucket )->tuple;
         }

      Tuple< Key, Value > *Find( const Key& k ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) enty.  If the key is not already
         // in the hash, return nullptr.

         // Your code here.
         Bucket< Key, Value > *bucket = *helperFind( k, hashFunc( k ) );

         // If not nullptr, entry was found so returning reference to tuple...
         return bucket ? &bucket->tuple : nullptr;
         } // end Find()


      //Invalidates any Iterators / pointers
      // Requires hash table to be non-empty
      void Optimize( double loadFactor = 0.5 ) // does this imply load factor reaching this point?
         {
         // Modify or rebuild the hash table as you see fit
         // to improve its performance now that you know
         // nothing more is to be added.

         // It might be the case that the bucket size is far lower than expected
         // So it might be necessary to shrink the table size
         size_t expectedTS = static_cast<double>(numberOfBuckets) / loadFactor;

         std::vector< Bucket< Key, Value > *> flattened = flattenHashTable();
         delete []buckets;
         
         // Sort so that most frequent words are inserted first
         std::sort( flattened.begin(), flattened.end(), 
            []( Bucket< Key, Value > *lhs, Bucket< Key, Value > *rhs ) 
               { return lhs->tuple.value > rhs->tuple.value; } );

         // Doubles number of buckets and computes the two's power ceiling
         // .e.g computeTwosPowCeiling( 100 * 2 ) = 256
         size_t newTbSize = computeTwosPowCeiling( (ssize_t) expectedTS );
         buckets = new Bucket< Key, Value> *[ newTbSize ];
         memset( buckets, 0, sizeof(Bucket< Key, Value > *) * newTbSize );

         // Set member variables to reflect increased hash table size
         numberOfBuckets = 0;
         tableSize = newTbSize;

         auto addToHT = [this]( Bucket< Key, Value > *bucket )
            {
            bucket->next = nullptr; // Remove any pointer relationship
            uint32_t index = bucket->hashValue & ( tableSize - 1 );
            Bucket< Key, Value > **bucketPtr = buckets + index;
            // Goes all the way to the end of linked list
            for ( ; *bucketPtr; bucketPtr = &( *bucketPtr )->next ); //iterates until end of linked list chain
            *bucketPtr = bucket;
            ++numberOfBuckets;
            };

         std::for_each( flattened.begin(), flattened.end(), addToHT );
         }

      // Your constructor may take as many default arguments
      // as you like.

      HashTable( size_t tb = DEFAULTSIZE ) : numberOfBuckets(0), tableSize(tb), buckets( new Bucket< Key, Value > *[ tb ] )
         {
         assert( tb );
         // Your code here.
         memset( buckets, 0, sizeof(Bucket< Key, Value > *) * tb );
         }

      ~HashTable( )
         {
         // Your code here.
         for ( Bucket< Key, Value > **bucket = buckets, 
               ** const end = buckets + tableSize; bucket != end; ++bucket )
            delete *bucket;

         delete[] buckets;             
         } // end ~HashTable()


      std::vector< const Bucket< Key, Value> * > constflattenHashTable()
         {
         Bucket< Key, Value > const *currBucket = *buckets;
         Bucket< Key, Value > **mainLevel = buckets;
         std::vector< const Bucket< Key, Value> * > bucketVec;
         bucketVec.reserve( numberOfBuckets );

         for ( Bucket< Key, Value > **const end = buckets + tableSize; 
               mainLevel != end && bucketVec.size() < numberOfBuckets; ++mainLevel )
            {
            for ( currBucket = *mainLevel ; currBucket ; currBucket = currBucket->next )
               bucketVec.emplace_back( currBucket );
            }  

         return bucketVec;
         }
      
      void swap( HashTable& other )
         {
         APESEARCH::swap( buckets, other.buckets );
         APESEARCH::swap( tableSize, other.tableSize );
         APESEARCH::swap( numberOfBuckets, other.numberOfBuckets );
         APESEARCH::swap( collisions, other.collisions );
         APESEARCH::swap( hashFunc, other.hashFunc );
         } 

      inline size_t size() const { return numberOfBuckets; }
      inline size_t table_size() const { return tableSize; }

      class Iterator
         {
         private:

            friend class HashTable;
            // Your code here.
            HashTable *table; // for the tableSize
            //Bucket< Key, Value > *currentBucket;
            Bucket< Key, Value > **currentBucket;
            Bucket< Key, Value > **mainLevel;

            Iterator( HashTable *_table, size_t bucket ) :  table( _table ), mainLevel( table->buckets + bucket ) {
               for (  Bucket< Key, Value > **end = table->buckets + table->tableSize
                  ; mainLevel != end && !(*mainLevel); ++mainLevel ); // Order of comparison is required: *mainLevel could be an invalid read if mainLevel == end
               currentBucket = mainLevel;
            }

            // This constructor can be used for finds that want to return an iterator instead
            Iterator( HashTable *_table, Bucket<Key, Value> **b ) :  table( _table ), currentBucket( b ), 
                  mainLevel( table ? table->buckets + ( *b )->hashValue & ( table->tableSize - 1 ) : nullptr ) {}

            Iterator( HashTable *_table, size_t bucketInd, Bucket<Key, Value> *b ) :  table( _table ), mainLevel( table->buckets + bucketInd )
               {
               // Your code here.
               Bucket< Key, Value > *bucket = table->buckets[bucketInd];
               for( ; bucket && bucket != b; bucket = bucket->next );
               
               if( bucket ) 
                  {
                  currentBucket = b;
                  return;
                  } // end if

               mainLevel = table->buckets + table->tableSize;
               currentBucket = *mainLevel;
               }

            void advanceBucket( )
               {
               if ( ( *currentBucket )->next )
                  currentBucket = & ( *currentBucket )->next;
               else
                  {
                  ++mainLevel; // Go to next place
                  // Order of comparasions matter since *(buckets + tableSize) isn't owned by us 
                  // Need to find a top-level bucket that's valid
                  for ( Bucket< Key, Value > **end = table->buckets + table->tableSize
                     ; mainLevel != end && !(*mainLevel); ++mainLevel ); 
                  currentBucket = mainLevel;
                  }
               } // end advanceBucket()

         public:

            Iterator( )// : Iterator( nullptr, 0, nullptr )
               {
               table = nullptr;
               currentBucket = nullptr;
               mainLevel = nullptr;
               }

            ~Iterator( )
               {
               }

            Tuple< Key, Value > &operator*( )
               {
               // Your code here.
               return ( *currentBucket )->tuple;
               }

            Tuple< Key, Value > *operator->( ) const
               {
               // Your code here.
               return & ( *currentBucket )->tuple;
               }

            // Prefix ++
            Iterator &operator++( )
               {
               advanceBucket();
               return *this;
               }

            // Postfix ++
            Iterator operator++( int )
               {
               Iterator old( *this );
               advanceBucket();
               return old;
               }

            bool operator==( const Iterator &rhs ) const
               {
               return currentBucket == rhs.currentBucket;
               }

            bool operator!=( const Iterator &rhs ) const
               {
               return currentBucket != rhs.currentBucket;
               }
         };

      Iterator begin( )
         {
         // Your code here.
         //Iterator(table, 0, table->buckets[])
         return Iterator( this, size_t ( 0 ) );
         }

      Iterator end( )
         {
         return Iterator( this, tableSize );
         }
      
      Iterator FindItr( const Key& k ) const
         {
         Bucket< Key, Value > **bucket = helperFind( k, hashFunc( k ) );

         return *bucket ? Iterator( this, bucket ) : end();
         }
      
      void OptimizeElegant( double loadFactor = 0.5 )
         {
         size_t expectedTS = static_cast<double>(numberOfBuckets) / loadFactor;
         size_t newTbSize = computeTwosPowCeiling( (ssize_t) expectedTS );

         HashTable temp ( newTbSize );

         for ( Iterator itr = begin(); itr != end(); ++itr )
            {
            Tuple <Key, Value> *pair = &( *itr.currentBucket )->tuple;
            temp.Find( pair->key, pair->value );
            }
         swap( temp );
         }
   };
