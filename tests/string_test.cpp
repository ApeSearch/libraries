#ifdef STD
    #include <string>
    using std::string;
#else // Used for Autograder
    #include "../string.h"
#endif

#include <iostream>

int main() {
    // Test Default Constructor
    string empty, empty_cmp;
    if (empty != empty_cmp)
        std::cout << "Failed Default Constructor compare to string: " << empty << " " << std::endl;
    if (empty != "")
        std::cout << "Failed Default Constructor compare to cstring: " << empty << " " << std::endl;

    // Test cstr constructor
    string one = "hello";
    if (one != "hello")
        std::cout << "Failed c-string Constructor: " << one << " " << std::endl;

    // Test Nullchar cstr constructor
    string null("");
    if (null != "")
        std::cout << "Failed Null character Constructor" << null << " " << std::endl;

    // Test size()
    if (one.size() != 5)
        std::cout << "Failed size(): " << one << " " << std::endl;

    // Test operator +=
    string base = "help";
    string appendThis = " me";
    base += appendThis;
    if (base != "help me")
        std::cout << "Failed += operator: " << base << " " << std::endl;

    // Test pushBack()
    #ifdef STD
        // cmoooon Hamilton use the same interface smh
        base.push_back('A');
    #else
        base.pushBack('A');
    #endif
    if (base != "help meA")
        std::cout << "Failed pushBack: " << base << " " << std::endl;

    // Test popBack()
    #ifdef STD
        base.pop_back();
    #else
        base.popBack();
    #endif
    if (base != "help me")
        std::cout << "Failed popBack: " << base << " " << std::endl;
    
    // Test relational operators, these tests are not good; they check that it "works", no edge cases
    string left = "left";
    string right = "right";
    if (left > right)
        std::cout << "Failed > operator: " << std::endl;
    if (left >= right)
        std::cout << "Failed >= operator: " << std::endl;
    if (!(left < right))
        std::cout << "Failed < operator: " << std::endl;
    if (!(left <= right))
        std::cout << "Failed <= operator: " << std::endl;
    if (left == right)
        std::cout << "Failed == operator: " << std::endl;
    if (!(left != right))
        std::cout << "Failed != operator: " << std::endl;
}