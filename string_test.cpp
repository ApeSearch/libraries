#include <iostream>

#include "string.h"
using nstd::string;

int main()
   {
   string t1;
   string t2( "test2" );
   string t3 = "test3";
   string t4 = "ayyyyyylmao";

   t1 = t3;
   t1 = t4;
   t4 = t2;

   std::cout << t1;
   std::cout << " ";
   std::cout << t2;
   std::cout << " ";
   std::cout << t3;
   std::cout << std::endl;
   }