
#pragma once

#ifndef UNORDERED_MAP_APE_SEARCH
#define UNORDERED_MAP_APE_SEARCH
//------------------------------------------------------------------------------------------------
//
//                                  Algorithms Declaration
//
//------------------------------------------------------------------------------------------------
// A bunch of functions that aid in creating efficent, well know algorithms.

#include <cctype>
#include <utility>

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

int stricmp(const char *a, const char *b) {
    int letterA, letterB;

    do {
        // Accounts for non-2's complement as well as 2's coplmenet platforms.
        // Since +0 and -0 would fold to a unsigned 0 but in reality only +0 should be converted to 0.
        letterA = *((unsigned char *)a++);
        letterB = *((unsigned char *)b++);
        letterA = tolower(toupper(letterA));
        letterB = tolower(toupper(letterB));
    }
    while ( letterA == letterB && letterA != '\0');
    return letterA - letterB;
}

template<typename T> void swap(T& lhs, T& rhs)
   {
   T temp = std::move( lhs );
   lhs = std::move( rhs );
   rhs = std::move( temp );
   }
} // end namespace APESEARCH


#endif // end ALGORITHMS_H_APE_SEARCH