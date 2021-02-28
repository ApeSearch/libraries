#include "../vector.h"
#include <vector>

#include <iostream>
using std::cout;
using std::endl;


void testConstructors();

int main() {
    vector<int> testVector;

    if (testVector.size() != 0 || testVector.capacity() != DEFAULT_BUCKET_SIZE)
        {
        cout << "Vector Default constructor failed" << endl;
        }



    
    testVector.pushBack(0);
    if (testVector.size() != 1 || testVector.capacity() != 8)
        {
        cout << "Vector pushBack failed: " << testVector.capacity() << endl;
        }
    if (testVector[0] != 0) 
        {
        cout << "Vector [] operator failed" << endl;
        }

    for (int i = 1; i <= DEFAULT_BUCKET_SIZE; ++i) 
        {
            testVector.pushBack(i);
            if (testVector.size() != (i+1))
                {
                cout << "Vector size increase failed: " << testVector.capacity() << endl;
                }
            if (testVector[i] != i) 
                {
                cout << "Vector []/pushBack operator failed: " << testVector[i] << endl;
                }
        }

    if (testVector.size() != (DEFAULT_BUCKET_SIZE + 1) || testVector.capacity() != (DEFAULT_BUCKET_SIZE * 2))
        {
            cout << "Vector multiple pushBack failed: " << testVector.capacity() << endl;
        }

    // Test popBack
    const size_t len = testVector.size();
    for (size_t i = 0; i < len; ++i) 
        {
        testVector.popBack();
        }

    if (testVector.size() != 0)
        {
        cout << "Vector popBack failed: " << testVector.size() << endl;
        }

    testConstructors();

}


void testConstructors() 
    {
    
        {
        vector<int> testVector( 0 );

        if ( testVector.capacity() != DEFAULT_BUCKET_SIZE && testVector.size() )
            {
                cout << "Vector size constructor for vector failed" << endl;
            }
        }
    
        {
        vector<int> testVector( 1 );

        if ( testVector.capacity() != DEFAULT_BUCKET_SIZE && testVector.size() != 1 && testVector.begin() )
            {
                cout << "Vector size constructor for vector failed" << endl;
            }
        
        vector<int> testVector1( 15 );
        if ( testVector1.capacity() != 16 && testVector1.size() != 15 )
            {
                cout << "Vector size constructor for vector failed" << endl;
            }
        for ( size_t n = 0; n < testVector1.size() ; ++n )
        {
            if ( testVector1[ n ] )
            {
                cout << "Vector fill constructor failed got : " <<  testVector1[ n ]  << " instead of 0" << endl;
                break;
            }
        }
        } // End Test


        {
        vector<int> testVector( 10 , 1 );
        if ( testVector.capacity() != 16 && testVector.size() != 10 )
            {
            cout << "Vector size constructor for vector failed" << endl;
            }
        for ( size_t n = 0; n < testVector.size() ; ++n )
            {
            if ( testVector[ n ] != 1 )
                {
                cout << "Vector fill constructor failed got : " <<  testVector[ n ]  << " instead of 0" << endl;
                break;
                }
            } // end for
        }

    } // end testConstructor()