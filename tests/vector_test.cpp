#include "../vector.h"
#include <vector>

#include <iostream>
using std::cout;
using std::endl;

int main() {
    vector<int> testVector;

    if (testVector.size() != 0 || testVector.capacity() != 0)
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

    for (int i = 1; i <= BUCKET_SIZE; ++i) 
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

    if (testVector.size() != (BUCKET_SIZE + 1) || testVector.capacity() != (BUCKET_SIZE * 2))
        {
            cout << "Vector multiple pushBack failed: " << testVector.capacity() << endl;
        }

    const size_t len = testVector.size();
    for (size_t i = 0; i < len; ++i) 
        {
        testVector.popBack();
        }

    if (testVector.size() != 0)
        {
        cout << "Vector popBack failed: " << testVector.size() << endl;
        }
}