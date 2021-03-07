
#pragma once

#ifndef UNORDERED_MAP_APE_SEARCH
#define UNORDERED_MAP_APE_SEARCH
//------------------------------------------------------------------------------------------------
//
//                                  Algorithms Declaration
//
//------------------------------------------------------------------------------------------------
// A bunch of functions that aid in creating efficent, well know algorithms.

#include <cctype> // For tolower and toupper
#include <utility> // for std::move

namespace APESEARCH
{
// Returns an iterator to value that is not less than value provided. (It could be either the last value or pointing to the value itself)
template<class ForwardIterator, class T, class Predicate>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val, Predicate pred) {
    while (first < last) {
        ForwardIterator mid = first + (last - first) / 2;
        if (pred(*mid, val))
            first = mid + 1;
        else  // obj[mid] >= val
            last = mid;
    } // end while
    return first;
} // end lower_bound()

int stricmp(const char *a, const char *b);

template<typename T> void swap(T& lhs, T& rhs) noexcept
   {
   T temp = std::move( lhs );
   lhs = std::move( rhs );
   rhs = std::move( temp );
   }

template<class InputIterator, class OutputIterator>
OutputIterator copy( InputIterator first, const InputIterator last, OutputIterator destIter )
   {
   while( first != last )
       *destIter++ = *first++;
    return destIter;
   }

template<class InputIterator, class Function>
Function for_each( InputIterator first, InputIterator last, Function fn )
   {
   while (first!=last)
      fn ( *first++ );
   return std::move( fn );
   }


} // end namespace APESEARCH
#endif // end ALGORITHMS_H_APE_SEARCH