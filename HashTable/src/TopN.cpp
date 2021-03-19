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

//TODO maybe don't use pairing heap.
Pair **TopN( Hash *hashtable, int N )
   {
   // Find the top N pairs based on the values and return
   // as a dynamically-allocated array of pointers.  If there
   // are less than N pairs in the hash, remaining pointers
   // will be null.

   Pair **pairArray = new Pair *[ unsigned( N ) ]();

   std::priority_queue <Pair*, std::vector<Pair *>, greaterComparator> pq;
   for ( Hash::Iterator it = hashtable->begin(); it != hashtable->end(); ++it )
      {
      pq.push( &*it );

      if(pq.size() > N)
         pq.pop();
      } // end ofr
   
   while(!pq.empty())
      {
      pairArray[pq.size() - 1] = pq.top();
      pq.pop(); 
      }
   return pairArray;
   }
