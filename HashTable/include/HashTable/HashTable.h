// Simple hash table template.

// Nicole Hamilton  nham@umich.edu

#pragma once

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>
#include <cstring> // for strlen
#include <assert.h>
#include <algorithm>
#include <vector>
using std::sort;

#define DEFAULTSIZE 8

static inline size_t computeTwosPowCeiling(size_t num) 
   {
   size_t powerNum = 1;
   for (; num; num >>=1 )
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




//using namespace std;


// You may add additional members or helper functions.


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
      Hash hashFunc;

      friend class Iterator;
      friend class HashBlob;

   Bucket< Key, Value > **helperFind( const Key& k, uint32_t hashVal ) const
      {
         // Applying a bit-wise mask on the least-sig bits
         uint32_t index = hashVal & ( tableSize - 1 );
         Bucket< Key, Value > **bucket = buckets + index;

         for ( ; *bucket; bucket = &( *bucket )->next )
            {
            if( ( *bucket )->tuple.key == k )
               break;
            }
         return bucket;
      } // end helperFind()
   
   void advanceBucket( Bucket< Key, Value > ***currBucket, Bucket< Key, Value> ***mainLevel )
      {
      if ( ( **currBucket )->next )
         *currBucket = &( **currBucket )->next;
      else
         *currBucket = ++( *mainLevel );
      } // end advanceBucket()


   std::vector< Bucket< Key, Value> *> flattenHashTable()
      {
      Bucket< Key, Value > *currBucket = *buckets, **mainLevel = buckets;
      std::vector< Bucket< Key, Value> * > bucketVec;
      bucketVec.reserve( numberOfBuckets );

      for ( Bucket< Key, Value > **const end = buckets + tableSize; 
            mainLevel != end && bucketVec.size() < numberOfBuckets;  )
         {
         for ( ; currBucket ; currBucket = currBucket->next )
            bucketVec.emplace_back( currBucket );
         currBucket = *++mainLevel;
         }  

      return bucketVec;
      }  // end flattenHashTable()

   public:
   std::vector< const Bucket< Key, Value> * > constflattenHashTable()
      {
      Bucket< Key, Value > const *currBucket = *buckets;
      Bucket< Key, Value > **mainLevel = buckets;
      std::vector< const Bucket< Key, Value> * > bucketVec;
      bucketVec.reserve( numberOfBuckets );

      for ( Bucket< Key, Value > **const end = buckets + tableSize; 
            mainLevel != end && bucketVec.size() < numberOfBuckets;  )
         {
         for ( ; currBucket ; currBucket = currBucket->next )
            bucketVec.emplace_back( currBucket );
         currBucket = *++mainLevel;
         }  

      return bucketVec;
      }

      Tuple< Key, Value > *Find( const Key k, const Value initialValue )
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
            } // end if
      
         return & ( * bucket )->tuple;
         }

      Tuple< Key, Value > *Find( const Key k ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) enty.  If the key is not already
         // in the hash, return nullptr.

         // Your code here.
         Bucket< Key, Value > *bucket = *helperFind( k, hashFunc( k ) );

         if( bucket && bucket->tuple.key == k )
            return &bucket->tuple;

         return nullptr;

         } // end Find()


      //Invalidates any Iterators / pointers
      void Optimize( double loading = 1.5 ) // does this imply load factor reaching this point?
         {
         // Modify or rebuild the hash table as you see fit
         // to improve its performance now that you know
         // nothing more is to be added.

         // Your code here.

         if ( loading > static_cast<double>(numberOfBuckets) / tableSize )
            return;

         std::vector< Bucket< Key, Value > * > flattened = flattenHashTable();
         std::sort( flattened.begin(), flattened.end(), 
            []( Bucket< Key, Value > *lhs, Bucket< Key, Value > *rhs ) 
               { return lhs->tuple.value > rhs->tuple.value; } );
         delete []buckets;

         size_t newTbSize = computeTwosPowCeiling( numberOfBuckets << 1 );
         buckets = new Bucket< Key, Value> *[ newTbSize ];
         numberOfBuckets = 0;
         tableSize = newTbSize;

         auto pred = [this]( Bucket< Key, Value > *val )
            {
            val->next = nullptr; // Remove any pointer relationship
            uint32_t index = val->hashValue & ( tableSize - 1 );
            Bucket< Key, Value > **bucket = buckets + index;
            for ( ; *bucket; bucket = &( *bucket )->next );
            *bucket = val;
            };

         std::for_each( flattened.begin(), flattened.end(), pred );
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


      class Iterator
         {
         private:

            friend class HashTable;
            //HashTable *table; // table_size
            // Your code here.
            HashTable *table;
            Bucket< Key, Value > *current_bucket;
            size_t current_hash;
            
            //if(!current_bucket->next)
            //   current_bucket = table->buckets[current_hash++];

            Iterator( HashTable *_table, size_t bucket, Bucket<Key, Value> *b ) :  table( _table ), current_hash( bucket )
               {
               // Your code here.
               
               }

         public:

            Iterator( ) : Iterator( nullptr, 0, nullptr )
               {
               }

            ~Iterator( )
               {
               }

            Tuple< Key, Value > &operator*( )
               {
               // Your code here.
               }

            Tuple< Key, Value > *operator->( ) const
               {
               // Your code here.
               }

            // Prefix ++
            Iterator &operator++( )
               {
               // Your code here.
               }

            // Postfix ++
            Iterator operator++( int )
               {
               // Your code here.
               }

            bool operator==( const Iterator &rhs ) const
               {
               // Your code here.
               return true;
               }

            bool operator!=( const Iterator &rhs ) const
               {
               // Your code here.
               return true;
               }
         };

      Iterator begin( )
         {
         // Your code here.
         }

      Iterator end( )
         {
         // Your code here. table->buckets + table->tabSize;
         }
   };
