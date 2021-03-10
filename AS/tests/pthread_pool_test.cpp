#include <iostream>
#include <random>
#include <functional> // for std::bind

#include "../include/AS/pthread_pool.h"
using APESEARCH::PThreadPool;
#include<unistd.h>
#include<thread>
#include <iostream>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_int_distribution<int> dist(-1000, 1000);
auto rnd = std::bind(dist, mt);


void simulate_hard_computation() {
  //std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}

// Simple function that adds multiplies two numbers and prints the result
void multiply(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
}

// Same as before but now we have an output parameter
void multiply_output(int & out, const int a, const int b) {
  simulate_hard_computation();
  out = a * b;
  std::cout << a << " * " << b << " = " << out << std::endl;
}

// Same as before but now we have an output parameter
int multiply_return(const int a, const int b) {
  simulate_hard_computation();
  const int res = a * b;
  std::cout << a << " * " << b << " = " << res << std::endl;
  return res;
}


int main()
{
  // Create pool with X threads
  PThreadPool pool(100u, APESEARCH::defer_init);

  // Initialize pool
  pool.init();

  // Submit (partial) multiplication table
  for (int i = 1; i < 10; ++i) {
    for (int j = 1; j < 100; ++j) {
      pool.submit(multiply, i, j);
    }
  }

  // Submit function with output parameter passed by ref
  int output_ref;
  auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);
  std::cout << "Submitted future1\n";

  // Wait for multiplication output to finish
  
  auto future2 = pool.submit(multiply_return, 5, 3);
  std::cout << "Submitted future2\n";

  std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
  if ( !future1.valid() )
     {
      std::cout << "valid for future1 is false!" << std::endl;
     }
  else {
    std::cout << "Everything's fine for future1 here\n";
  }

  future1.get();

  std::cout << "After future1\n";
  if ( !future1.valid() )
     {
      std::cout << "valid is false for future1!" << std::endl;
     }
  else {
    std::cout << "Everything's fine with future1\n";
  }
  std::cout << "Last operation result is equals to " << output_ref << std::endl;

  // Submit function with return parameter 

  std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
  std::cout << "Checking future2\n";
  if ( !future2.valid() )
     {
      std::cout << "valid is false for future2!" << std::endl;
     }
  else {
    std::cout << "Everything's fine with future2\n";
  }
  // Wait for multiplication output to finish
  int res = future2.get();
  std::cout << "Last operation result is equals to " << res << std::endl;
  
  pool.shutdown();
  pool.shutdown();

  return 0;
}