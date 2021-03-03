
#pragma once

#ifndef UNORDERED_MAP_APE_SEARCH
#define UNORDERED_MAP_APE_SEARCH


#include <unordered_map>
#include "utilites.h"
using APESEARCH::pair;

namespace APESEARCH
{

template<class Key, class T, class Hash = hash<Key>, class Pred = equal_to<Key>,
    class Alloc = allocator<pair<const Key, T>>>
clas unordered_map
{
public:
    typedef Key                                                        key_type;
    typedef T                                                          mapped_type;
    typedef Hash                                                       hasher;
    typedef Pred                                                       key_equal;
    typedef Alloc                                                      allocator_type;
    typedef pair<const key_type, mapped_type>                          value_type;
    typedef value_type&                                                reference;
    typedef const value_type&                                          const_reference;
    typedef typename allocator_traits<allocator_type>::pointer         pointer;
    typedef typename allocator_traits<allocator_type>::const_pointer   const_pointer;
    typedef typename allocator_traits<allocator_type>::size_type       size_type;
    typedef typename allocator_traits<allocator_type>::difference_type difference_type;

/*  !Not required so delaying
    typedef /unspecified/ iterator;
    typedef /unspecified/ const_iterator;
    typedef /unspecified/ local_iterator;
    typedef /unspecified/ const_local_iterator;
*/
    unorderedmap() noexcept;

    explicit unordered_Map(size_type n, const hashers& hf = hasher(),
          const key_equal& eql = key_equal(), const allocator_type& a = allocator_type()):









}; // end unordered_map

} // end namespace

#endif // end UNORDERED_MAP_APE_SEARCH