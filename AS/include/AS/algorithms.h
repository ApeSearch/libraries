#pragma once

#ifndef ALGORITHMS_APE_SEARCH
#define ALGORITHMS_APE_SEARCH
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
    ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val, Predicate pred)
    {
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

    template<typename Predicate>
    inline const char * findChars( const char *str, const char *end, Predicate func)
    {
    while(str != end && !func(*str)) { ++str; }
    return str;
    } // end findChars()


    template<typename T> void swap( T& lhs, T& rhs ) noexcept
        {
        T temp ( std::move( lhs ) );
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
    while ( first != last )
        fn ( *first++ );
    return std::move( fn );
    }

    template<class T>
    const T& min(const T& a, const T&b ) { return ( b < a ) ? b : a; }

    template<class T, class Comparator>
    const T& min(const T& a, const T& b, Comparator comp ) { return comp(b, a) ? b : a; }

    template<class InputIterator, class UnaryPredicate>
    bool all_of( InputIterator first, InputIterator last, UnaryPredicate pred )
    {
    while( first != last )
        if ( !pred( *first++ ) )
            return false;
    return true;
    }

    template<class InputIterator, class T>
    InputIterator find( InputIterator first, InputIterator end, const T& val )
    {
    for ( ; first != end; ++first )
        if ( *first == val )
            return first;
    return end;
    } // end find()


} // end namespace APESEARCH

#endif // end ALGORITHMS_H_APE_SEARCH

/*
template <class _Compare, class _RandomAccessIterator>
void sort(_RandomAccessIterator __first, _RandomAccessIterator __last, _Compare __comp)
{
    // _Compare is known to be a reference type
    typedef typename std::iterator_traits<_RandomAccessIterator>::difference_type difference_type;
    typedef typename std::iterator_traits<_RandomAccessIterator>::value_type value_type;
    const difference_type __limit = std::is_trivially_copy_constructible<value_type>::value &&
                                    std::is_trivially_copy_assignable<value_type>::value ? 30 : 6;
    while (true)
    {
    __restart:
        difference_type __len = __last - __first;
        switch (__len)
        {
        case 0:
        case 1:
            return;
        case 2:
            if (__comp(*--__last, *__first))
                swap(*__first, *__last);
            return;
        case 3:
            _VSTD::__sort3<_Compare>(__first, __first+1, --__last, __comp);
            return;
        case 4:
            _VSTD::__sort4<_Compare>(__first, __first+1, __first+2, --__last, __comp);
            return;
        case 5:
            _VSTD::__sort5<_Compare>(__first, __first+1, __first+2, __first+3, --__last, __comp);
            return;
        }
        if (__len <= __limit)
        {
            _VSTD::__insertion_sort_3<_Compare>(__first, __last, __comp);
            return;
        }
        // __len > 5
        _RandomAccessIterator __m = __first;
        _RandomAccessIterator __lm1 = __last;
        --__lm1;
        unsigned __n_swaps;
        {
        difference_type __delta;
        if (__len >= 1000)
        {
            __delta = __len/2;
            __m += __delta;
            __delta /= 2;
            __n_swaps = _VSTD::__sort5<_Compare>(__first, __first + __delta, __m, __m+__delta, __lm1, __comp);
        }
        else
        {
            __delta = __len/2;
            __m += __delta;
            __n_swaps = _VSTD::__sort3<_Compare>(__first, __m, __lm1, __comp);
        }
        }
        // *__m is median
        // partition [__first, __m) < *__m and *__m <= [__m, __last)
        // (this inhibits tossing elements equivalent to __m around unnecessarily)
        _RandomAccessIterator __i = __first;
        _RandomAccessIterator __j = __lm1;
        // j points beyond range to be tested, *__m is known to be <= *__lm1
        // The search going up is known to be guarded but the search coming down isn't.
        // Prime the downward search with a guard.
        if (!__comp(*__i, *__m))  // if *__first == *__m
        {
            // *__first == *__m, *__first doesn't go in first part
            // manually guard downward moving __j against __i
            while (true)
            {
                if (__i == --__j)
                {
                    // *__first == *__m, *__m <= all other elements
                    // Parition instead into [__first, __i) == *__first and *__first < [__i, __last)
                    ++__i;  // __first + 1
                    __j = __last;
                    if (!__comp(*__first, *--__j))  // we need a guard if *__first == *(__last-1)
                    {
                        while (true)
                        {
                            if (__i == __j)
                                return;  // [__first, __last) all equivalent elements
                            if (__comp(*__first, *__i))
                            {
                                swap(*__i, *__j);
                                ++__n_swaps;
                                ++__i;
                                break;
                            }
                            ++__i;
                        }
                    }
                    // [__first, __i) == *__first and *__first < [__j, __last) and __j == __last - 1
                    if (__i == __j)
                        return;
                    while (true)
                    {
                        while (!__comp(*__first, *__i))
                            ++__i;
                        while (__comp(*__first, *--__j))
                            ;
                        if (__i >= __j)
                            break;
                        swap(*__i, *__j);
                        ++__n_swaps;
                        ++__i;
                    }
                    // [__first, __i) == *__first and *__first < [__i, __last)
                    // The first part is sorted, sort the secod part
                    // _VSTD::__sort<_Compare>(__i, __last, __comp);
                    __first = __i;
                    goto __restart;
                }
                if (__comp(*__j, *__m))
                {
                    swap(*__i, *__j);
                    ++__n_swaps;
                    break;  // found guard for downward moving __j, now use unguarded partition
                }
            }
        }
        // It is known that *__i < *__m
        ++__i;
        // j points beyond range to be tested, *__m is known to be <= *__lm1
        // if not yet partitioned...
        if (__i < __j)
        {
            // known that *(__i - 1) < *__m
            // known that __i <= __m
            while (true)
            {
                // __m still guards upward moving __i
                while (__comp(*__i, *__m))
                    ++__i;
                // It is now known that a guard exists for downward moving __j
                while (!__comp(*--__j, *__m))
                    ;
                if (__i > __j)
                    break;
                swap(*__i, *__j);
                ++__n_swaps;
                // It is known that __m != __j
                // If __m just moved, follow it
                if (__m == __i)
                    __m = __j;
                ++__i;
            }
        }
        // [__first, __i) < *__m and *__m <= [__i, __last)
        if (__i != __m && __comp(*__m, *__i))
        {
            swap(*__i, *__m);
            ++__n_swaps;
        }
        // [__first, __i) < *__i and *__i <= [__i+1, __last)
        // If we were given a perfect partition, see if insertion sort is quick...
        if (__n_swaps == 0)
        {
            bool __fs = _VSTD::__insertion_sort_incomplete<_Compare>(__first, __i, __comp);
            if (_VSTD::__insertion_sort_incomplete<_Compare>(__i+1, __last, __comp))
            {
                if (__fs)
                    return;
                __last = __i;
                continue;
            }
            else
            {
                if (__fs)
                {
                    __first = ++__i;
                    continue;
                }
            }
        }
        // sort smaller range with recursive call and larger with tail recursion elimination
        if (__i - __first < __last - __i)
        {
            _VSTD::__sort<_Compare>(__first, __i, __comp);
            // _VSTD::__sort<_Compare>(__i+1, __last, __comp);
            __first = ++__i;
        }
        else
        {
            _VSTD::__sort<_Compare>(__i+1, __last, __comp);
            // _VSTD::__sort<_Compare>(__first, __i, __comp);
            __last = __i;
        }
    }
}
*/

