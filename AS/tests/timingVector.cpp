#include <iostream>
using std::cout;
using std::endl;
#include <chrono>
#include "../include/AS/vector.h"
#include <vector>


class Timer
    {
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio< 1 > >;

    std::chrono::time_point<clock_t> m_begin;
public:
    Timer( ) : m_begin( clock_t::now( ) ) { }

    void reset( )
       {
        m_begin = clock_t::now( );
       }
    double elapsed( ) const
       {
        return std::chrono::duration_cast< second_t > ( clock_t::now( ) - m_begin ).count( );
       }
    };


template< typename object>
void timePushBack( object& obj )
    {
    Timer t;
    for( size_t n = 0; n < 1000000; ++n )
        {
        obj.push_back( n );
        } // end for
    std:: cout << t.elapsed( ) << " seconds elapsed\n";
    }

int main( ) 
    {
    
    APESEARCH::vector< size_t > vecAS;
    std::vector< size_t > stdVec;

    cout << "Testing stdVec\n";
    
    timePushBack( stdVec );

    cout << "Testing AS vector\n";
    timePushBack( vecAS );

    return 0;
    }
