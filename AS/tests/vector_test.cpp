#include "../include/vector.h"
#include <vector>
using APESEARCH::vector;

#include <iostream>
using std::cout;
using std::endl;


void testConstructors();

int main() {
    vector<int> testVector;

    if (testVector.size() != 0 || testVector.capacity() != 0 )
        {
        cout << "Vector Default constructor failed" << endl;
        }



    
    testVector.push_back(0);
    if (testVector.size() != 1 || testVector.capacity() != 8)
        {
        cout << "Vector push_back failed: " << testVector.capacity() << endl;
        }
    if (testVector[0] != 0) 
        {
        cout << "Vector [] operator failed" << endl;
        }

    for (size_t i = 1; i <= DEFAULT_BUCKET_SIZE; ++i) 
        {
            testVector.push_back(static_cast<int>(i));
            if (testVector.size() != (i+1))
                {
                cout << "Vector size increase failed: " << testVector.capacity() << endl;
                }
            if (testVector[i] != static_cast<int>(i)) 
                {
                cout << "Vector []/push_back operator failed: " << testVector[i] << endl;
                }
        }

    if (testVector.size() != (DEFAULT_BUCKET_SIZE + 1) || testVector.capacity() != (DEFAULT_BUCKET_SIZE * 2))
        {
            cout << "Vector multiple push_back failed: " << testVector.capacity() << endl;
            cout << "Size: " << testVector.size() << '\n';
            cout << "Capacity: " << testVector.capacity() << endl;
        }

    // Test popBack
    const size_t len = testVector.size();
    for (size_t i = 0; i < len; ++i) 
        {
        testVector.pop_back();
        }

    if (testVector.size() != 0)
        {
        cout << "Vector pop_back failed: " << testVector.size() << endl;
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
        vector<int> testCopyOP = testVector;

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



        {
        vector<int> testVector( 10 , 1 );

        vector<int> movedTestVector( std::move( testVector ) );


        vector<int> defaultVector;

        cout << "Checking move constructor capacity and size" << endl;
        if ( testVector.capacity() != defaultVector.capacity() || testVector.size() != defaultVector.size() )
            {
            cout << "Move Constructor failed... TestVector shoudl've been left in a default state" << endl;
            cout << "orig Capacity: " << testVector.capacity() << endl;
            cout << "orig size: " << testVector.size() << endl;
            exit(1);
            }
        cout << "Checking that elements in orignal vector has been changed to default state" << endl;
        for ( size_t n = 0; n < testVector.size(); ++n )
            {
            if ( testVector[ n ] )
                {
                cout << "Move Constructor failed... TestVector shoudl've been left in a default state" << endl;
                exit(1);
                } //end if
            } // end for


        }

    } // end testConstructor()