#pragma once

#ifndef QUEUE_H_APESEARCH
#define QUEUE_H_APESEARCH

#include <deque>
#include <type_traits> // for std::is_nothrow_swappable
#include "algorithms.h" // for swap

#include "vector.h"
#include <type_traits>
#include "utility.h"
#include "assert.h"

namespace APESEARCH
{

      template <class T, class Container = std::deque<T>>
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

          void swap(queue& q) noexcept
      #if __cplusplus >= 201703L
          (std::is_nothrow_swappable<Container>() )
      #endif
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
          virtual ~ApesearchPQ() {}
          virtual void push( const T& val ) noexcept = 0;
          virtual void push( T&& val ) noexcept = 0;
          virtual void pop() noexcept = 0;
          virtual const T& top() const = 0;
          virtual std::size_t size() const = 0;
          virtual bool empty() const = 0;
          virtual void make_heap() = 0; // Restores heap invariant
      };
      template<typename T>
      class less
      {
      public:
          constexpr bool operator()(const T &lhs, const T &rhs) const 
          {
          return lhs < rhs;
          }
      };

      template<class T, typename COMP = less<T> >
      class BinaryPQ;


      template <class T, class Container = BinaryPQ<T, less<T> >, class Compare = less<T> >
      class priority_queue 
      {
      public:
          typedef Container                                container_type;
          //typedef typename container_type::value_type      value_type;
          //typedef typename container_type::reference       reference;
          //typedef typename container_type::const_reference const_reference;
          //typedef typename container_type::size_type       size_type;

      protected:
          container_type cont;

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
          inline std::size_t size() const { return cont.size(); }
          inline const T& top() const { return cont.top(); }

          inline void push(const T& v) { cont.push( v ); }
          inline void push( T&& v) { cont.push( std::forward<T> ( v ) ); }
          template <class... Args> void emplace(Args&&... args) { cont.emplace( std::forward<Args> ( args )... ); }
          void pop() { cont.pop(); }

          void swap(priority_queue& q)
      #if __cplusplus >= 201703L
              noexcept(std::is_nothrow_swappable_v<Container> &&
                      std::is_nothrow_swappable_v<Compare>)
      #endif
          ;
      };

      #if __cplusplus >= 201703L
      template <class Compare, class Container>
      priority_queue(Compare, Container)
          -> priority_queue<typename Container::value_type, Container, Compare>; // C++17
      #endif
      template <class T, class Container, class Compare>
        void swap(priority_queue<T, Container, Compare>& x,
                  priority_queue<T, Container, Compare>& y)
                  noexcept(noexcept(x.swap(y)));

      #include "queue.inl"

}  // end APESEARCH


#endif