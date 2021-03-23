// TopN.cpp
// Nicole Hamilton nham@umich.edu

// Given a hashtable, return a dynamically-allocated array
// of pointers to the top N pairs based on their values.
// Caller is responsible for deleting the array.

// Individual pairs are to be C-strings as keys and size_t's
// as values.

#ifdef LOCAL
   #include "../include/HashTable/HashTable.h"
   #include "../include/HashTable/TopN.h"
#else
   #include "HashTable.h"
   #include "Common.h"
#endif

using namespace std;
#include <queue>
#include <vector>

#include <utility>     // You'll need swap() eventually
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

template<class Comparator>
Pair ** insertSortN( Pair **pairArray, Pair **pairValidEnd, Pair ** pairTrueEnd, Pair *tuple, Comparator comp )
   {
   assert( pairValidEnd != pairTrueEnd );
   Pair **itr = pairValidEnd;
   *itr = tuple;
   // while tuple > currentVal
   for ( ;itr != pairArray && comp( *itr, *( itr - 1 ) ); --itr )
      std::swap( *itr, *(itr - 1) );
   
   return pairValidEnd + 1 == pairTrueEnd ? pairValidEnd : pairValidEnd + 1;
   } // end inserSortN()

//TODO maybe don't use pairing heap.
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


