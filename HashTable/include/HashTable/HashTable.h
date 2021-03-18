// Simple hash table template.

// Nicole Hamilton  nham@umich.edu

#pragma once

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>

#define DEFAULTSIZE 8

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
      //uint32_t hashValue;
      Tuple< Key, Value > tuple;

      Bucket( const Key &k, const Value v ) :
            tuple( k, v ), next( nullptr ) //, hashValue( h )
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
      //Bucket< Key, Value > *buckets[DEFAULTSIZE];
      size_t tableSize; // length of bucket array
      size_t numberOfBuckets; // Contains amount of seperate chained buckets
      Hash hashFunc;

      friend class Iterator;
      friend class HashBlob;

   inline bool checkNext( Bucket< Key, Value > **bucket ) const
      {
      assert( bucket );
      if ( ( *bucket )->next  ) // -> higher than *
         {
         *bucket = ( *bucket )->next;
         return true;
         } // end if
      return false;
      }

   Tuple< uint32_t, Bucket< Key, Value > * > helperFind( const Key& k ) const
      {
         // Applying a bit-wise mask on the least-sig bits
         uint32_t index = hashFunc( k ) & ( tableSize - 1 );
         Bucket< Key, Value > *bucket = buckets[ index ];

         for ( ; bucket; checkNext( &bucket ) )
            {
            if( bucket->tuple.key == k )
               break;
            }
         return Tuple< uint32_t,  Bucket< Key, Value >* >( index, bucket );
      } // end helperFind()

   public:

      Tuple< Key, Value > *Find( const Key k, const Value initialValue )
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) entry.  If the key is not already
         // in the hash, add it with the initial value.

         // Your code here
         Tuple< uint32_t,  Bucket< Key, Value >* > bucket = helperFind( k );
         
         // Checks for nullptr
         if( !bucket.value )
            {
            Bucket< Key, Value > **bucketPtr = buckets + bucket.key; // Pointer to buckets + index
            *bucketPtr = new Bucket< Key, Value >( k, initialValue ); // Directly modify value within (two layers)
            ++numberOfBuckets;
            return & ( *bucketPtr )->tuple; // Follow all pointers to the value
            } // end if
         // Checks if value is same or not (if not implies that reached end of linked list)
         else if( bucket.value->tuple.key != k )
            {
            bucket.value->next = new Bucket< Key, Value >( k, initialValue );
            ++numberOfBuckets;
            return &( bucket.value->next->tuple );
            } // end else if
      
         // Was able to successfully find the value in this case
         return &bucket.value->tuple;

         }

      Tuple< Key, Value > *Find( const Key k ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) enty.  If the key is not already
         // in the hash, return nullptr.

         // Your code here.
         Tuple< uint32_t,  Bucket< Key, Value >* > bucket = helperFind( k );

         if( bucket.value && bucket.value->tuple.key == k )
            return &bucket.value->tuple;

         return nullptr;

         } // end Find()


      //Invalidates any Iterators / pointers
      void Optimize( double loading = 1.5 )
         {
         // Modify or rebuild the hash table as you see fit
         // to improve its performance now that you know
         // nothing more is to be added.

         // Your code here.
         }


      // Your constructor may take as many default arguments
      // as you like.
      

      HashTable( size_t tb = DEFAULTSIZE ) : numberOfBuckets(0), tableSize(tb), buckets( new Bucket< Key, Value > *[ tb ] )
         {
         // Your code here.
         memset( buckets, 0, sizeof(Bucket< Key, Value > *) * tb );
         
         }

      ~HashTable( )
         {
         // Your code here.
         for ( Bucket< Key, Value >** bucket = buckets; bucket != buckets + tableSize; ++bucket )
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
