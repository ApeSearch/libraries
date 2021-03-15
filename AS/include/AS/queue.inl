
template<class T, class Container, class Compare>
priority_queue<T, Container, Compare>::priority_queue( const Compare& comp ) : compare( comp ) {}

template<class T, class Container, class Compare>
priority_queue<T, Container, Compare>::priority_queue(const Compare& comp, const Container& c) : cont( c ), compare( comp ) {}

template<class T, class Container, class Compare>
template <class InputIterator>
priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last,
        const Compare& comp) : cont ( first, last ), compare( comp ) {}


template<class T, class Container, class Compare>
template <class InputIterator>
priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last,
        const Compare& comp, const Container& c) : cont ( c ( first, last ) ), compare( comp ) {}

template<class T, class Container, class Compare>
template <class InputIterator>
priority_queue<T, Container, Compare>::priority_queue(InputIterator first, InputIterator last,
                       const Compare& comp, Container&& c) : 
        priority_queue<T, Container, Compare>::priority_queue<InputIterator>( first, last, comp, c ) {}

