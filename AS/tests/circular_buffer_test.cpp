
#include <iostream>
using std::cout;
#include <stdlib.h>
#include <assert.h>

#include "../include/AS/circular_buffer.h"
using APESEARCH::circular_buffer;

template<typename T, class buffer_type>
void print_buffer_status(const circular_buffer<T, buffer_type>& cbuf)
   {
    cout << "Full: " << std::boolalpha << cbuf.full() << ", ";
    cout << "empty: " << cbuf.empty() << ", ";
    cout << "size: " << cbuf.size() << '\n';
    cout << cbuf << '\n';
   }

int main(void)
{
	printf("\n=== C++ Circular Buffer Check ===\n");

    // Testing with initializer List
    circular_buffer<size_t> cbuf({1,2,3,4,5,6,7,8});

	printf("Buffer initialized. ");

    print_buffer_status( cbuf );

    // Reset
    printf("Resetting...\n");
    cbuf.reset();

    print_buffer_status( cbuf );

	printf("\n******\nAdding %lu values\n", APESEARCH::DEFAULT::capacity - 1);
	for(size_t i = 0; i < APESEARCH::DEFAULT::capacity - 1; i++)
	{
        cbuf.putOverwrite( i );
		printf("Added %lu, Size now: %lu\n", i, cbuf.size() );
	}
    print_buffer_status( cbuf );

	printf("\n******\nAdding %lu values\n",  APESEARCH::DEFAULT::capacity );
	for(size_t i = 0; i < APESEARCH::DEFAULT::capacity; i++)
	{
        cbuf.putOverwrite( i );
		printf("Added %lu, Size now: %lu\n", i, cbuf.size() );
	}

    print_buffer_status( cbuf );

	printf("\n******\nReading back values: ");
	while( !cbuf.empty() )
	{
		size_t data = cbuf.get();
		printf("%lu ", data);
	}
	printf("\n");

    print_buffer_status( cbuf );

	printf("\n******\nAdding %lu values\n",  APESEARCH::DEFAULT::capacity + 5);
	for(size_t i = 0; i <  APESEARCH::DEFAULT::capacity + 5; i++)
	{
	    cbuf.putOverwrite( i );
		printf("Added %lu, Size now: %zu\n", i, cbuf.size() );
	}

    print_buffer_status( cbuf );

	printf("\n******\nReading back values: ");
	while( !cbuf.empty() )
	{
		size_t data = cbuf.get();
		printf("%lu ", data);
	}
	printf("\n");

	printf("\n******\nAdding %lu values using non-overwrite version\n", APESEARCH::DEFAULT::capacity + 5);
	for(size_t i = 0; i < APESEARCH::DEFAULT::capacity + 5; i++)
	{
		cbuf.put( i );
	}

    print_buffer_status( cbuf );

	printf("\n******\nReading back values: ");
	while( !cbuf.empty() )
	{
		size_t data = cbuf.get();
		printf("%lu ", data);
	}
	printf("\n\nFinal Buffer State\n");

    print_buffer_status( cbuf );

	return 0;
}
