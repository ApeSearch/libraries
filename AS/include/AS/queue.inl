
template<class T, class Container, class Compare>
explicit priority_queue::priority_queue( const Compare& comp ) : compare( comp ) {}

template<class T, class Container, class Compare>
priority_queue::priority_queue(const Compare& comp, const container_type& c) : cont( c ), compare( comp ) {}

template <class InputIterator>
template<class T, class Container, class Compare>
priority_queue::priority_queue(InputIterator first, InputIterator last,
        const Compare& comp = Compare()) : cont ( first, last ), compare( comp ) {}


template <class InputIterator>
template<class T, class Container, class Compare>
priority_queue::priority_queue(InputIterator first, InputIterator last,
        const Compare& comp, const container_type& c) : cont ( c ( first, last ) ), compare( comp ) {}

template <class InputIterator>
template<class T, class Container, class Compare>
priority_queue::priority_queue(InputIterator first, InputIterator last,
                       const Compare& comp, container_type&& c) : ApesearchPQ::ApesearchPQ( first, last, comp, c ) {}

template<class T, class Container, class Compare>
