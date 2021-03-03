#pragma once

#ifndef ALLOCATOR_APE_SEARCH
#define ALLOCATOR_APE_SEARCH


/*
 * THe purpose of ALlocator is to contruct copies of objects. 
*/

template<typename T>
class Allocator
{

public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;


    // For converting differnt allocator to a different type
    // Specically T => U type
    typename<typename U>
    struct rebind 
       {
       typedef ALlocator<U> other;
       };

    inline explicit Allocator() {}
    inlince ~Allocator() {}
    inline explicit Allocator(Alloctor const&) {}
    template<typename U>
    inline explicit Allocator(Allocator<U> const&) {}

    // Getting pointer
    inline value_type* address(value_type& r) { return &r; }
    inline value_type const* address(const value_type& rc) { return &rc; }

    //! About reinterpret_cast
    //!It is used to convert one pointer of another pointer of any type, no matter either the class is related to each other or not.
    //!It does not check if the pointer type and data pointed by the pointer is same or not.
    // memory allocation
    inline value_type* allocator(size_type numOf,
       typename std::allocator<void>::const_pointer = 0) 
       {
       return reinterpret_cast<value_type*>( ::operator new(numOf * sizeof( value_type )) );
       }   
    inline void deallocate(value_type *p, size_type)
        {
        ::operator delete( p );
        }


    // size (see how many T objects can be made with size_t)
    inline size_type max_size() const 
       {
       return std::numeric_limits<size_type>::max() / sizeof( T );
       }


    // construction/destruction
    // Think how int (p = new int (25) => new (p) int(25) )
    inline void construct( value_type *p, const T& t ) { new( p ) T( t ); }
    inline void destroy( value_type *p ) { p->~T(); }


    inline bool operator==( Allocator const& ) { return true; }
    inline bool operator!=( Allocator const& a) { return !operator==(a); }
}; // end Allocator




#endif // end ALLOCATOR_APE_SEARCH





