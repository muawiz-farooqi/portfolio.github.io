#include "mavalloc.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main( int argc, char * argv[] )
{
	clock_t begin = clock();

	unsigned char * array[10000];
	unsigned char * ptr2;
	unsigned char * ptr3;
	unsigned char * ptr4;

	// for (int j = 0; j < 2; j++)
	{
		for(int i = 0; i < 10000; i++)
		{
			array[i] = malloc(20);
		}

		for(int i = 0; i < 10000; i++)
		{
			free(array[i]);
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 15)
			{
				array[i] = malloc(20);
			}
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 15)
			{
				free(array[i]);
			}
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 1)
			{
				array[i] = malloc(20);
			}
		}

		for(int i = 0; i < 10000; i++)
		{
			if (i % 1)
			{
				free(array[i]);
			}
		}
	}

	for (int j = 0; j < 10000; j++)
	{
		ptr2 = malloc(9000);
		ptr3 = malloc(100);

		free(ptr2);
		
		ptr4 =  malloc(90);
		
		free(ptr4);
		free(ptr3);
	}

	clock_t end = clock();
	double time_spent = (double)(end - begin) / (CLOCKS_PER_SEC/1000);
	printf("runtime for benchmark5: %fms\n", time_spent);

	return 0;
}
