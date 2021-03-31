// TopN.cpp
// Nicole Hamilton nham@umich.edu

// Given a hashtable, return a dynamically-allocated array
// of pointers to the top N pairs based on their values.
// Caller is responsible for deleting the array.

// Individual pairs are to be C-strings as keys and size_t's
// as values.

#ifdef LOCAL
   #include "../include/HashTable/HashTable.h" // Includes swap
   #include "../include/HashTable/TopN.h"
#else
   #include "HashTable.h"
   #include "Common.h"
#endif

using namespace std;

#include <assert.h>

using Hash = HashTable< const char *, size_t >;
using Pair = Tuple< const char *, size_t >;

class greaterComparator
   {
public:
   bool operator()( const Pair *left, const Pair *right )
      {
      return left->value > right->value;
      }
   };

// OO version
template<class Comparator>
class TopNFinder
{
   Pair **pairArray;
   Pair **validEnd;
   Pair ** end;
   size_t numOfVals;
   Comparator comp;

public:

   TopNFinder( const size_t n ) : pairArray( new Pair*[ n + 1 ]() ), validEnd( pairArray ), end( pairArray + n + 1 ), numOfVals( n ) {}
   ~TopNFinder()
      {
      delete[] pairArray;
      }

   void operator()( Pair *tuple )
      {
      assert( validEnd != end );
      Pair **itr = validEnd;
      *itr = tuple; // Set it to the end

      for ( ;itr != pairArray && comp( *itr, *( itr - 1 ) ); --itr )
         swap( *itr, *( itr -1 ) );

      if ( validEnd + 1 != end )
         ++validEnd;
      } // end inserSortN()
   inline Pair ** release() 
      { 
      Pair **rawData = pairArray;
      pairArray = nullptr;
      return rawData; 
      }
};

// Procedural Version
template<class Comparator>
Pair ** insertSortN( Pair **pairArray, Pair **pairValidEnd, Pair ** pairTrueEnd, Pair *tuple, Comparator comp )
   {
   assert( pairValidEnd != pairTrueEnd );
   Pair **itr = pairValidEnd;
   *itr = tuple;
   // while tuple > currentVal
   for ( ;itr != pairArray && comp( *itr, *( itr - 1 ) ); --itr )
      swap( *itr, *(itr - 1) );
   
   return pairValidEnd + 1 == pairTrueEnd ? pairValidEnd : pairValidEnd + 1;
   } // end inserSortN()

Pair **TopN( Hash *hashtable, int N )
   {
   // Find the top N pairs based on the values and return
   // as a dynamically-allocated array of pointers.  If there
   // are less than N pairs in the hash, remaining pointers
   // will be null.

   Pair **pairArray = new Pair *[ unsigned( N + 1 ) ]();
   Pair **validEnd = pairArray;
   Pair ** end = pairArray + unsigned( N + 1 );
   greaterComparator comp;

   for ( Hash::iterator it = hashtable->begin(); it != hashtable->end(); ++it )
      validEnd = insertSortN< greaterComparator >( pairArray, validEnd, end, &*it, comp );
   
   return pairArray;
   }


