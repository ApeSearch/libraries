
#include "../include/unique_ptr.h"
#include "assert.h"
#include <iostream>

class testClass {};
class testClass1: public testClass {};


int main()
{

    APESEARCH::unique_ptr<testClass> test(new testClass);
    APESEARCH::unique_ptr<testClass1> test1(new testClass1);


    test = std::move(test1);


                                            // foo   bar    p
                                            // ---   ---   ---
    APESEARCH::unique_ptr<int> foo;                // null
    APESEARCH::unique_ptr<int> bar;                // null  null
    int* p = nullptr;                        // null  null  null

    foo = APESEARCH::unique_ptr<int>(new int(10)); // (10)  null  null
    bar = std::move(foo);                    // null  (10)  null
    p = bar.get();                           // null  (10)  (10)
    assert( *p == 10 );
    *p = 20;                                 // null  (20)  (20)
    assert( *bar == 20 );
    p = nullptr;                             // null  (20)  null

    foo = APESEARCH::unique_ptr<int>(new int(30)); // (30)  (20)  null
    assert( *foo == 30 );
    p = foo.release();                       // null  (20)  (30)
    assert( !foo ); // foo now points to nullptr
    assert( *p == 30 );
    *p = 40;                                 // null  (20)  (40)

    std::cout << "foo: ";
    if (foo) std::cout << *foo << '\n'; else std::cout << "(null)\n";

    std::cout << "bar: ";
    if (bar) std::cout << *bar << '\n'; else std::cout << "(null)\n";

    std::cout << "p: ";
    if (p) std::cout << *p << '\n'; else std::cout << "(null)\n";
    std::cout << '\n';

    delete p;   // the program is now responsible of deleting the object pointed to by p
                // bar deletes its managed object automatically

    return 0;
}