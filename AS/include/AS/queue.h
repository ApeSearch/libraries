#pragma once

#ifndef QUEUE_H_APESEARCH
#define QUEUE_H_APESEARCH

#ifdef testing
    #include <deque>
    using std::deque;
#else
    #include "deque.h"
    using APESEARCH::deque;
#endif
#include <type_traits> // for std::is_nothrow_swappable
#include "algorithms.h" // for swap

#include "vector.h"
#include <fuctional>
#include <queue>

namespace APESEARCH
{

template <class T, class Container = deque<T>>
class queue
{
public:
    typedef Container                                container_type;
    typedef typename container_type::value_type      value_type;
    typedef typename container_type::reference       reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type       size_type;
    //static_assert((is_same<T, value_type>::value), "" );

protected:
    container_type c;

public:
    queue() = default;
    ~queue() = default;

    queue(const queue& q) = default;
    queue(queue&& q) = default;

    queue& operator=(const queue& q) = default;
    queue& operator=(queue&& q) = default;

    explicit queue(const container_type& _c) : c( _c ) {}

    explicit queue(container_type&& _c) : c( std::move( _c ) ) {}

    /*    
    template <class Alloc>
        explicit queue(const Alloc& a);
    template <class Alloc>
        queue(const container_type& c, const Alloc& a);
    template <class Alloc>
        queue(container_type&& c, const Alloc& a);
    template <class Alloc>
        queue(const queue& q, const Alloc& a);
    template <class Alloc>
        queue(queue&& q, const Alloc& a);
    */

    bool      empty() const { return c.empty(); }
    size_type size() const { return c.size(); }

    reference front() { return c.front(); }
    const_reference front() const { return c.front(); }
    //reference       back() { return c.back(); } Not allowed
    const_reference back() const { return c.back(); }

    void push(const value_type& v) { c.push_back( v ); }
    void push(value_type&& v) { c.push_back( std::move( v ) ); }

    template <class... Args> 
    decltype( auto ) emplace(Args&&... args)
    {
        return c.emplace_back( std::forward<Args>( args )... );
    }
    void pop() { c.pop_front(); }

    void swap(queue& q) noexcept(std::is_nothrow_swappable<Container>() )
       {
        APESEARCH::swap( c, q.c );
       } // end swap()
    
};

#if __cplusplus >= 201703L
template<class Container>
  queue(Container) -> queue<typename Container::value_type, Container>; // C++17

template<class Container, class Allocator>
  queue(Container, Allocator) -> queue<typename Container::value_type, Container>; // C++17
#endif

template <class T, class Container>
  bool operator==(const queue<T, Container>& x,const queue<T, Container>& y) { return x.c == y.c; }

template <class T, class Container>
  bool operator< (const queue<T, Container>& x,const queue<T, Container>& y) { return x.c < y.c; }

template <class T, class Container>
  bool operator!=(const queue<T, Container>& x,const queue<T, Container>& y) { return x.c != y.c; }

template <class T, class Container>
  bool operator> (const queue<T, Container>& x,const queue<T, Container>& y) { return x.c > y.c; }

template <class T, class Container>
  bool operator>=(const queue<T, Container>& x,const queue<T, Container>& y) { return x.c >= y.c; }

template <class T, class Container>
  bool operator<=(const queue<T, Container>& x,const queue<T, Container>& y) { return x.c <= y.c; }

template <class T, class Container>
  void swap(queue<T, Container>& x, queue<T, Container>& y)
  noexcept(noexcept(x.swap(y)));


// Apesearch's comparator interface
template<typename T, typename comparator>
class ApesearchPQ {
protected:
    comparator compare;
    ApesearchPQ() {}
    explicit ApesearchPQ( const comparator &comp ) : compare( comp ) {}
public:
    virtual void push( const T& val ) noexcept = 0;
    virtual void pop() noexcept = 0;
    virtual const T& top() const = 0;
    virtual std::size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual ~ApesearchPQ();
    virtual make_heap(); // Restores heap invariant
};


template <class T, class Container = vector<T>,
          class Compare = std::less<typename Container::value_type> >
class priority_queue 
{
public:
    typedef Container                                container_type;
    typedef typename container_type::value_type      value_type;
    typedef typename container_type::reference       reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::size_type       size_type;

protected:
    container_type cont;
    Compare compare;

public:
    priority_queue() = default;
    ~priority_queue() = default;

    priority_queue(const priority_queue& q) = default;
    priority_queue(priority_queue&& q) = default;

    priority_queue& operator=(const priority_queue& q) = default;
    priority_queue& operator=(priority_queue&& q) = default;

    explicit priority_queue(const Compare& comp);
    priority_queue(const Compare& comp, const container_type& c);
    explicit priority_queue(const Compare& comp, container_type&& c);
    template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last,
                       const Compare& comp = Compare());
    template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last,
                       const Compare& comp, const container_type& c);
    template <class InputIterator>
        priority_queue(InputIterator first, InputIterator last,
                       const Compare& comp, container_type&& c);

    inline bool            empty() const { return cont.empty(); }
    inline size_type       size() const { return cont.size(); }
    inline const_reference top() const { return cont.top(); }

    inline void push(const value_type& v) { cont.push( v ); }
    inline void push(value_type&& v) { cont.push( std::forward<value_type> ( v ) ); }
    //template <class... Args> void emplace(Args&&... args) { cont.push(   ) }
    void pop() { cont.pop(); }

    void swap(priority_queue& q)
        noexcept(is_nothrow_swappable_v<Container> &&
                 is_nothrow_swappable_v<Comp>)
};

template <class Compare, class Container>
priority_queue(Compare, Container)
    -> ApesearchPQ<typename Container::value_type, Container, Compare>; // C++17

template<class InputIterator,
         class Compare = less<typename iterator_traits<InputIterator>::value_type>,
         class Container = vector<typename iterator_traits<InputIterator>::value_type>>
priority_queue(InputIterator, InputIterator, Compare = Compare(), Container = Container())
    -> ApesearchPQ<typename iterator_traits<InputIterator>::value_type, Container, Compare>; // C++17

template<class Compare, class Container, class Allocator>
priority_queue(Compare, Container, Allocator)
    -> ApesearchPQ<typename Container::value_type, Container, Compare>; // C++17

template <class T, class Container, class Compare>
  void swap(ApesearchPQ<T, Container, Compare>& x,
            ApesearchPQ<T, Container, Compare>& y)
            noexcept(noexcept(x.swap(y)));

#include "queue.inl"

}  // end APESEARCH


#endif