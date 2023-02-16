#include "mavalloc.h"
#include <time.h>
#include <stdio.h>


int main( int argc, char * argv[] )
{
	clock_t begin = clock();

	mavalloc_init(10000, WORST_FIT);
	unsigned char * array[10000];
	unsigned char * ptr2;
	unsigned char * ptr3;
	unsigned char * ptr4;

	// for (int j = 0; j < 2; j++)
	{
		for(int i = 0; i < 10000; i++)
		{
			array[i] = mavalloc_alloc(20);
		}

		for(int i = 0; i < 10000; i++)
		{
			mavalloc_free(array[i]);
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 15)
			{
				array[i] = mavalloc_alloc(20);
			}
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 15)
			{
				mavalloc_free(array[i]);
			}
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 1)
			{
				array[i] = mavalloc_alloc(20);
			}
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 1)
			{
				mavalloc_free(array[i]);
			}
		}
	}

	for (int j = 0; j < 10000; j++)
	{
		ptr2 = mavalloc_alloc(9000);
		ptr3 = mavalloc_alloc(100);

		mavalloc_free(ptr2);
		
		ptr4 =  mavalloc_alloc(90);
		
		mavalloc_free(ptr4);
		mavalloc_free(ptr3);
	}

	clock_t end = clock();
	double time_spent = (double)(end - begin) / (CLOCKS_PER_SEC/1000);
	printf("runtime for benchmark2: %fms\n", time_spent);

	return 0;
}

