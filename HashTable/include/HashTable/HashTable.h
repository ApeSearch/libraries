// Simple hash table template.

#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>
#include <cstring> // for strlen
#include <assert.h>
#include <algorithm> // for std::sort
#include <vector>
using std::sort;
#include "../../../AS/include/AS/algorithms.h" // for APESEARCH::swap
#include "../../../AS/include/AS/utility.h" // for APESEARCH::pair
#include <stdlib.h>
#include <time.h>


#define DEFAULTSIZE 4096
#define MAX 16430 // Golden Random number credited by MC

static inline size_t computeTwosPow( ssize_t num, bool computeCeiling = true ) 
   {
   num-= computeCeiling; // Account for num already being a two's power
   size_t powerNum = 1;
   for (; num > ( ssize_t )!computeCeiling; num >>=1 )
      powerNum <<= 1;
   return powerNum;
   }

class FNV
{
public:
   FNV() = default;
   virtual ~FNV() {}
   virtual size_t operator()( const char *data ) const
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

class CStringComparator
{
public:

   bool operator()( const char *L, const char *R ) const
      {
      return !strcmp( L, R );
      }

};


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
            next( nullptr ) , hashValue( h ), tuple( k, v ) 
         {
         }
      ~Bucket()
         {
         //Recursive delete on bucket-chain
         delete next;
         } // end ~Bucket()
  };

template< typename Key, typename Value>
class PerfectHashing : public FNV
{

   size_t tableSize; // where things can be put 
   std::vector<ssize_t> arr; // equal to tableSize...

  size_t operator()( const char *data, size_t num = 0 ) const 
      {
      size_t length = strlen(data);

      static const size_t FnvOffsetBasis=146959810393466560;
      static const size_t FnvPrime=1099511628211ul;
      if ( !num ) { num = FnvPrime; }
      size_t hash = FnvOffsetBasis;
      for ( size_t i = 0; i < length; ++i )
         {
         hash *= num;
         hash ^= (unsigned long)data[ i ];
         } // end for
      return hash;
      } // end operator()()

public:
   PerfectHashing( size_t _tableSize ) : tableSize( _tableSize ), arr( tableSize )
      {
      }

   ~PerfectHashing()
      {
      }

   size_t operator()( const char *data ) const override
      {
      //TODO optimize 
      ssize_t val = arr[ operator()(data, 0) & ( tableSize - 1 ) ];

      if ( val < 0 )
         return size_t ( ( -val ) - 1 ); // Uses the negative to map to bucket
      
      return operator()( data, val );
      } //end operator()
   
   void removePtrs( std::vector< Bucket< Key, Value> **> &bucketsPlaced )
      {
      for ( Bucket< Key, Value> **bucket : bucketsPlaced )
         *bucket = nullptr;
      }

   void assertSingleVectInvariants( typename std::vector< std::vector< Bucket< Key, Value> *> >::iterator currBucket, typename std::vector< std::vector< Bucket< Key, Value> *> >::iterator end )
      {
      for ( ; currBucket != end; ++currBucket )
         assert( currBucket->size() == 1 );
      }
   
   // Assume buckets is sorted
   Bucket< Key, Value > ** buildInterTable( std::vector< std::vector< Bucket< Key, Value> *> >& buckets, size_t tbSize, size_t numOfBuckets )
      {
      size_t bucketsCnt = 0; assert( tableSize == tbSize );
      Bucket< Key, Value > **tableArray = new Bucket< Key, Value > *[ tbSize ]();
      typename std::vector< std::vector< Bucket< Key, Value> *> >::iterator bucketVec = buckets.begin();
      for ( ;bucketVec != buckets.end() && bucketVec->size() > 1; ++bucketVec )
         {
         ssize_t arg = 1;
         
         std::vector< Bucket< Key, Value> **> bucketsPlaced; // To keep track of where buckets are part since might need to retry...
         size_t tempOf = 0;

         typename std::vector< Bucket< Key, Value> *>::iterator bucket = bucketVec->begin();
         while ( bucket != bucketVec->end() )
            {
            std::vector< Bucket< Key, Value> *>& vec = *bucketVec;
            size_t ind = operator()( ( *bucket )->tuple.key, arg ) & ( tableSize - 1 );
            Bucket< Key, Value > ** mainLevel = tableArray +  ind;
            assert( mainLevel < tableArray + tbSize );
            if ( *mainLevel )
               {
               removePtrs( bucketsPlaced );
               bucketsPlaced.clear();
               ++arg;
               tempOf = 0;
               bucket = bucketVec->begin();
               } // end if
            else
               {
               ( *bucket )->next = nullptr;
               *mainLevel = *bucket;
               bucketsPlaced.emplace_back( mainLevel );
               ++tempOf;
               ++bucket;
               }
            } // end for
         assert( tempOf == bucketsPlaced.size() );
         size_t bucketVecSize = bucketVec->size();
         assert( tempOf == bucketVecSize );
         bucketsCnt += tempOf;

         // Update the intermediate array
         size_t hash = operator()( bucketVec->front()->tuple.key, 0 );
         size_t hashInd = hash & ( tableSize - 1 );
         arr[ hashInd ] = arg;
         } // end for

      // At this point bucketVec should only point to vectors of size one...
      if ( bucketVec == buckets.end() )
         return tableArray;

      assertSingleVectInvariants( bucketVec, buckets.end() );

      // Find all empty buckets now
      size_t bucketsRemaining = buckets.end() - bucketVec;
      std::vector< Bucket< Key, Value> **> freeBuckets;
      freeBuckets.reserve( bucketsRemaining );
      for ( Bucket< Key, Value > **mainLevel = tableArray; tableArray != tableArray + tbSize && bucketsRemaining ; ++mainLevel )
         {
         if ( !( *mainLevel ) )
            {
            freeBuckets.emplace_back( mainLevel );
            --bucketsRemaining;
            }
         } // end for
      assert( !bucketsRemaining );
      assert( freeBuckets.size() == buckets.end() - bucketVec );
      // Place all single buckets now into these empty slots
      for ( ;bucketVec != buckets.end(); ++bucketVec, freeBuckets.pop_back() )
         {
         bucketVec->front()->next = nullptr;
         *freeBuckets.back() = bucketVec->front();
         ssize_t arrayInd = freeBuckets.back() - tableArray;
         arrayInd = ( -arrayInd ) - 1;
         size_t hash = operator()( bucketVec->front()->tuple.key, 0 );
         size_t hashInd = hash & ( tableSize - 1 );
         arr[ hashInd ] = arrayInd;
         ++bucketsCnt;
         } // end for
      assert( numOfBuckets == bucketsCnt );
      return tableArray;
      }
};


template< typename Key, typename Value, class Hash = FNV, class Comparator = CStringComparator > class HashTable
   {
   private:

      // Your code here.

      size_t tableSize; // length of bucket array
      Bucket< Key, Value > **buckets;
      size_t numberOfBuckets; // Contains amount of seperate chained buckets
      size_t collisions = 0; // Tracks current collisions in hash_table
      Comparator compare;
      Hash *hashFunc = nullptr;

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
         if ( compare( ( *bucket )->tuple.key, k ) )
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
         uint32_t hashVal = (*hashFunc)( k );
         Bucket< Key, Value > **bucket = helperFind( k, hashVal );
         
         // Checks for nullptr
         if( !*bucket )
            {
            if ( buckets[ hashVal & ( tableSize - 1 ) ] )
               ++collisions;
            *bucket = new Bucket< Key, Value >( k, initialValue, hashVal );
            ++numberOfBuckets;
            } // end if
      
         return & ( * bucket )->tuple;
         }
      size_t getCollisions() const { return collisions; }

      Tuple< Key, Value > *Find( const Key& k ) const
         {
         // Search for the key k and return a pointer to the
         // ( key, value ) enty.  If the key is not already
         // in the hash, return nullptr.

         // Your code here.
         Bucket< Key, Value > *bucket = *helperFind( k, (*hashFunc)( k ) );

         // If not nullptr, entry was found so returning reference to tuple...
         return bucket ? &bucket->tuple : nullptr;
         } // end Find()


      //!Invalidates any Iterators / pointers
      // Modify or rebuild the hash table as you see fit
      // to improve its performance now that you know
      // nothing more is to be added.
      void Optimize( double loadFactor, bool computeCeiling = true ) // does this imply load factor reaching this point?
         {
         // It might be the case that the bucket size is far lower than expected
         // So it might be necessary to shrink the table size
         size_t expectedTS = size_t ( static_cast<double>(numberOfBuckets) / loadFactor );

         std::vector< Bucket< Key, Value > *> flattened = flattenHashTable();
         delete []buckets;
         
         // Sort so that most frequent words are inserted first
         std::sort( flattened.begin(), flattened.end(), 
            []( Bucket< Key, Value > *lhs, Bucket< Key, Value > *rhs ) 
               { return lhs->tuple.value > rhs->tuple.value; } );

         // Doubles number of buckets and computes the two's power ceiling
         // .e.g computeTwosPow( 100 * 2 ) = 256
         size_t newTbSize = computeTwosPow( (ssize_t) (expectedTS - 1), computeCeiling );

         // Adjust member variables
         buckets = new Bucket< Key, Value> *[ newTbSize ];
         memset( buckets, 0, sizeof(Bucket< Key, Value > *) * newTbSize );
         tableSize = newTbSize;
         collisions = 0;

         // Insert each bucket back into the table
         for ( Bucket< Key, Value > *bucket : flattened )
            {
            bucket->next = nullptr; // Remove any pointer relationship
            uint32_t index = bucket->hashValue & ( tableSize - 1 );
            Bucket< Key, Value > **bucketPtr = buckets + index;
            if ( *bucketPtr )
               ++collisions;
            // Goes all the way to the end of linked list
            for ( ; *bucketPtr; bucketPtr = &( *bucketPtr )->next ); //iterates until end of linked list chain
            *bucketPtr = bucket;
            } // end for
         }
      


      // Your constructor may take as many default arguments
      // as you like.

      HashTable( size_t tb = DEFAULTSIZE, Hash *hasher = new FNV(), Comparator comp = CStringComparator() ) : tableSize( computeTwosPow( (ssize_t)tb ) ), 
         buckets( new Bucket< Key, Value > *[ tableSize ] ), numberOfBuckets( 0 ), compare( comp ), hashFunc( hasher )
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

         delete hashFunc;
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
         APESEARCH::swap( compare, other.compare );
         } 

      inline size_t size() const { return numberOfBuckets; }
      inline size_t table_size() const { return tableSize; }
      inline double load_factor() const { return static_cast<double>( numberOfBuckets ) / tableSize; }
      inline double ratioOfColli() const { return static_cast<double>( collisions ) / numberOfBuckets; }

      inline size_t numOfLinkedLists() const
         {
         size_t numOfLL = 0;
         for ( Bucket< Key, Value > **mainLevel = buckets, **end = buckets + tableSize; mainLevel != end; ++mainLevel )
            if ( *mainLevel )
               ++numOfLL;
         return numOfLL;
         }

      bool advancePtr( Bucket< Key, Value > ***mainLevel, Bucket< Key, Value > **end, std::vector< APESEARCH::pair< Bucket< Key, Value > **, size_t > >& bucketVec ) const
         {
         for ( ; *mainLevel != end; ++(*mainLevel) )
            {
            if ( **mainLevel )
               {
               bucketVec.emplace_back( APESEARCH::pair< Bucket< Key, Value > **, size_t>( *mainLevel, 0 ) );
               return true;
               }
            } // end if
         return false;
         }
      
      bool advancePtr( Bucket< Key, Value > ***mainLevel, Bucket< Key, Value > **end, std::vector< std::vector< Bucket< Key, Value> *> >& bucketVec ) const
         {
         for ( ; *mainLevel != end; ++(*mainLevel) )
            {
            if ( **mainLevel )
               {
               bucketVec.emplace_back( std::vector< Bucket< Key, Value> *>() );
               return true;
               }
            } // end if
         return false;
         }
      
private:
      //! May be helpful when implementing Minimal perfect hash function
      // Returns a sparse vector of valid buckets and a pointer to the linked list and the amount of buckets within it.
      std::vector< APESEARCH::pair< Bucket< Key, Value > **, size_t> > linkedListOfBuckets() const
         {
         //Bucket< Key, Value > *currBucket = *buckets;
         Bucket< Key, Value > **mainLevel = buckets;
         Bucket< Key, Value > **const end = buckets + tableSize;
         std::vector< APESEARCH::pair< Bucket< Key, Value > **, size_t > > bucketVec;
         bucketVec.reserve( numOfLinkedLists() );

         for ( ;advancePtr( &mainLevel, end, bucketVec ); ++mainLevel )
            {
            APESEARCH::pair< Bucket< Key, Value > **, size_t >& bucketRef = bucketVec.back();
            for ( Bucket< Key, Value > *currBucket = *mainLevel ; currBucket ; currBucket = currBucket->next )
               ++bucketRef.second();
            } // end for
         return bucketVec;
         } 
      void assertLinkedList( std::vector< Bucket< Key, Value> *>& vec ) const
         {
         for ( typename std::vector< Bucket< Key, Value> *>::iterator currBucket = vec.begin(); currBucket != vec.end(); ++currBucket )
            {
            if ( currBucket + 1 == vec.end() )
               assert( !( ( *currBucket )->next ) );
            else 
               assert( ( *currBucket )->next == *( currBucket + 1 )  );
            }
         } // end assertLinkedList()
      std::vector< std::vector< Bucket< Key, Value> *> > vectorOfBuckets() const
         {
         Bucket< Key, Value > **mainLevel = buckets;
         Bucket< Key, Value > **const end = buckets + tableSize;
         std::vector< std::vector< Bucket< Key, Value> *> > bucketVec;
         bucketVec.reserve( numOfLinkedLists() );
         size_t bucketCnt = 0;
         for ( ;advancePtr( &mainLevel, end, bucketVec ); ++mainLevel )
            {
            std::vector< Bucket< Key, Value> *>& bucketRef = bucketVec.back();
            assert( !bucketRef.size() );
            for ( Bucket< Key, Value > *currBucket = *mainLevel ; currBucket ; currBucket = currBucket->next )
               {
               bucketVec.back().emplace_back( currBucket );
               ++bucketCnt;
               }
            assert( bucketRef.size() );
            bucketRef.shrink_to_fit();
            assertLinkedList( bucketVec.back() );
            } // end for
         assert( bucketCnt == numberOfBuckets );
         return bucketVec;
         } // end vectorOfBuckets()
public:
      double averageCollisonsPerBucket() const
         {
         if ( !numberOfBuckets )
            return 0;
         return numberOfBuckets / static_cast<double> ( numOfLinkedLists() );
         }

      void printStats() const 
         {
         std::cout << "Total Buckets Allocated: " << size() << '\n';
         std::cout << "Table Size: " << table_size() << '\n';
         std::cout << "load_factor: " << load_factor() << '\n';
         std::cout << "Percentage of Collisions: " << ratioOfColli() << '\n';
         std::cout << "Average Collisions per Bucket: " << averageCollisonsPerBucket() << '\n';
         }

      class Iterator
         {
         private:

            friend class HashTable;
            // Your code here.
            HashTable *table; // for the tableSize
            Bucket< Key, Value > **mainLevel;
            Bucket< Key, Value > **currentBucket;

            Iterator( HashTable *_table, size_t bucket ) :  table( _table ), mainLevel( table->buckets + bucket ) {
               for (  Bucket< Key, Value > **end = table->buckets + table->tableSize
                  ; mainLevel != end && !(*mainLevel); ++mainLevel ); // Order of comparison is required: *mainLevel could be an invalid read if mainLevel == end
               currentBucket = mainLevel;
            }

            // This constructor can be used for finds that want to return an iterator instead
            Iterator( HashTable *_table, Bucket<Key, Value> **b ) :  table( _table ), currentBucket( b ), 
                  mainLevel( table ? table->buckets + ( *b )->hashValue & ( table->tableSize - 1 ) : nullptr ) {}

            Iterator( HashTable *_table, size_t bucketInd, Bucket<Key, Value> *b ) :  table( _table ), mainLevel( table ? table->buckets + bucketInd : nullptr ), currentBucket( mainLevel )
               {
               // Your code here.
               if ( !table )
                  return;
               assert( bucketInd < table->tableSize );

               // Follows linked list until either reaches something or a nullptr
               for( ; *currentBucket && *currentBucket != b; currentBucket = & ( *currentBucket )->next );

               if ( ! ( *currentBucket ) )
                  currentBucket = mainLevel = table->buckets + table->tableSize;
               }

            inline void advanceBucket( )
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

            Iterator( ) : Iterator( nullptr, 0, nullptr )
               {
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

            Iterator operator+( ssize_t var )
               {
               Iterator copy( *this );
               for ( ssize_t n = 0; n < var; ++n )
                  copy.advanceBucket();
               return copy;
               }
         };

      Iterator begin( )
         {
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
      
      //! Warning while this is simple, it also isn't very efficient
      void OptimizeElegant( double loadFactor = 0.5 )
         {
         size_t expectedTS = size_t ( static_cast<double>(numberOfBuckets) / loadFactor );
         size_t newTbSize = computeTwosPow( (ssize_t) expectedTS );

         HashTable temp ( newTbSize );

         for ( Iterator itr = begin(); itr != end(); ++itr )
            {
            Tuple <Key, Value> *pair = &( *itr.currentBucket )->tuple;
            temp.Find( pair->key, pair->value );
            }
         swap( temp );
         }
      void Optimize()
         {
         Optimize( 0.5, true );
         
         if ( !collisions )
            return;
         // Now do perfect hashing
         std::vector< std::vector< Bucket< Key, Value> *> > bucketsVec = vectorOfBuckets();
         assert( bucketsVec.size() == numOfLinkedLists() );
         std::sort( bucketsVec.begin(), bucketsVec.end(), []( std::vector< Bucket< Key, Value> *>& lhs, std::vector< Bucket< Key, Value> *>& rhs ) { return lhs.size() > rhs.size(); } );
         delete hashFunc;
         delete []buckets;
         hashFunc = new PerfectHashing<Key, Value>( tableSize );
         buckets = dynamic_cast<PerfectHashing<Key, Value>*>(hashFunc)->buildInterTable( bucketsVec, tableSize, numberOfBuckets );
         size_t numOfLL = numOfLinkedLists();
         collisions = numberOfBuckets - numOfLL;
         assert( collisions == 0 );
         }
   };

   template<typename Key, typename Value >
   void swap( HashTable< Key, Value>& lhs, HashTable< Key, Value>& rhs )
      {
      lhs.swap( rhs );
      }

