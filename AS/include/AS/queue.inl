
template<class T, class Container, class Compare>
priority_queue<T, Container, Compare>::priority_queue( const Compare& comp ) : cont( comp ) {}

template<class T, class Container, class Compare>
priority_queue<T, Container, Compare>::priority_queue(const Compare& comp, const Container& c) : cont( c, comp ) {}

template<class T, class Container, class Compare>
template <class InputIterator>
priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last,
        const Compare& comp) : cont ( first, last, comp ) {}


template<class T, class Container, class Compare>
template <class InputIterator>
priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last,
        const Compare& comp, const Container& c) : cont ( c ( first, last ), comp ) {}

template<class T, class Container, class Compare>
template <class InputIterator>
priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last,
                       const Compare& comp, Container&& c) : 
        priority_queue<T, Container, Compare>::priority_queue<InputIterator>( first, last, comp, c ) {}

#include "vector.h"
#include "utility.h"
#include "assert.h"

inline static int alignDivision( const int index )
   {
   return index & 1 ? index : index - 1;
   }

/*
 *  Heap implemented with a vector to make a complete binary tree.
 *                     
 *                      0
 *                   /     \
 *                  1       2
 *                /   \   /   \
 *               3    4  5     6
 *             /
 *            7     
*/
template<class T, typename COMP >
class BinaryPQ : public ApesearchPQ<T, COMP>
{
   using BaseClass = ApesearchPQ<T, COMP>;
   vector<T> data;

   /*
    * REQUIRES: child < data.size()
    * MODIFIES: data
    *  EFFECTS: Starts at a node and fixes the heap invariant upwards.
    * 
    * Typically used for insertions where the child is put at the end and 
    * then swapped until its parents are higher priority
    * 
    *   The setup at the beginning is to ensure that when dividing by two, that the parent does
    *   align properly. For example, 4's parent is 1 but 4/2 gives 2. Therefore it's necessary
    *   to always check before dividing...
    */
   void fixUp( int child )
      {
        assert( child < ( int )data.size() );
        int parent = child & 1 ? child : child - 1;
        // While parent < child ( where values comparied is priority )
        for ( ; parent >= 1 && BaseClass::compare( data[ unsigned( parent >>= 1 ) ], data[ (unsigned) child ] );
              child = parent, parent = alignDivision( child ) )
           {
            swap( data[ (unsigned)parent ], data[ (unsigned)child ] );
           } // end for
        return;
      } // end fixUp()
   /*
    * REQUIRES: parent >= 0 && child < data.size()
    * MODIFIES: data
    *  EFFECTS: Restores a heap in which the heap invariant is violated. 
    * 
    * The main use for this function is to restore the heap efficiently after
    * an element has been popped from the top of the heap.
    * 
   */
   void fixDown( size_t parent )
      {
      size_t child = ( parent + 1 ) << 1;

      while( child < data.size() )
         {
          // First find the higher priority child ( it might get upgraded! )
          if ( BaseClass::compare( data[ child ], data[ child - 1 ] ) )
             --child; // left child has the higher priority
          if ( !BaseClass::compare( data[ parent ], data[ child ] ) )
             break;
        
          swap( data[ parent ], data[ child ] );
          parent = child;
          child = ( parent + 1 ) << 1;
         } // end while
      
      // To account for multiplication going to data.size()
      // This could've been in the while loop but to avoid doing log(n) comparisons,
      // was extracted
      if ( child == data.size() )
         {
         --child;
         if ( BaseClass::compare( data[ parent ], data[ child ] ) )
                swap( data[ parent ], data[ child ] );
         } // end if
      } // end fixDown()
public:
   //typedef typename BinaryPQ::T   value_type;
   //typedef typename value_type::&    reference;
   //typedef typename const reference const_reference;
   //typedef typename std::size_t       size_type;

   explicit BinaryPQ( COMP comp = COMP() ) : BaseClass( comp ) {}

   template<typename InputIterator>
   BinaryPQ( InputIterator start, InputIterator end, COMP comp = COMP() ) 
         : BaseClass{ comp }, data( start, end )
      {
       make_heap();
      } // end BinaryPQ()
   
   virtual ~BinaryPQ() {}

   virtual void make_heap() override
      {
      int n = int( data.size() ) - ( ( data.size() - 1 ) & 1 ? 1 : 2 );
      n >>= 1; // to start at the lowest parent...
      for( ; n >= 0; --n )
        fixDown( ( size_t )n );
      }

   virtual void push( const T& val ) noexcept override
      {
      size_t index = data.size();
      data.push_back( val );
      fixUp( (int) index );
      } // end push()
   virtual void push( T&& val ) noexcept override
      {
      emplace( std::forward<T>( val ) );
      } // end push()
   
   template< class ...Args>
   void emplace( Args&& ...args ) noexcept
      {
      size_t index = data.size();
      data.emplace_back( std::forward<Args>( args )... );// Pack is getting expanded whne esplison is other way...
      fixUp( index );
      }
   
   virtual const T& top() const override
      {
      return data.front();
      }
   
   virtual std::size_t size() const override
      {
      return data.size();
      }
   
   virtual bool empty() const override
      {
      return data.empty();
      }

   
   virtual void pop() noexcept override
      {
      swap( data.front(), data.back() );
      data.pop_back();
      fixDown( 0 );
      } // end pop()



   void checkHeapInvariant() const
      {
      size_t parent = 0;
      size_t child = ( parent + 1 ) << 1;

      while ( child < data.size() )
         {
         T& lChild = data[ child - 1 ];
         T& rChild = data[ child ];
         T& parVal = data[ parent ];
         assert( compare( lChild, parVal ) && compare( rChild, parVal ) );
         parent = child;
         child = ( parent + 1 ) << 1;
         } // end for

      if ( child == data.size() )
         {
         --child;
         assert( compare( data[ child ], data[ parent ] ) );
         }  // end if
      } // checkHeapInvariant()
};
