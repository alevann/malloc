#ifdef __WIN
#include <io.h>
#else
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "malloc.h"


int main(int argc, char *argv[])
{
	int size;
	void *ptr[10];
	int i;

	init_malloc();
	/*
	 * try mallocing four pieces, each 1/4 of total size
	 */
	size = MAX_MALLOC_SIZE / 4;

	ptr[0] = __malloc(size);
	if(ptr[0] == NULL)
	{
		printf("malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}

	malloc_free_list();
	printf("\n");

	ptr[1] = __malloc(size);
	if(ptr[1] == NULL)
	{
		printf("malloc of ptr[1] failed for size %d\n",
				size);
		exit(1);
	}

	malloc_free_list();
	printf("\n");

	ptr[2] = __malloc(size);
	if(ptr[2] == NULL)
	{
		printf("malloc of ptr[2] failed for size %d\n",
				size);
		exit(1);
	}

	malloc_free_list();
	printf("\n");

	/*
	 * this one should fail due to rounding
	 */
	ptr[3] = __malloc(size);
	if(ptr[3] == NULL)
	{
		printf("malloc of ptr[3] fails correctly for size %d\n",
				size);
	}

	malloc_free_list();
	printf("\n");

	/*
	 * free the first block
	 */
	__free(ptr[0]);

	malloc_free_list();
	printf("\n");

	/*
	 * free the third block
	 */
	__free(ptr[2]);

	malloc_free_list();
	printf("\n");

	/*
	 * now __free second block
	 */
	__free(ptr[1]);

	malloc_free_list();
	printf("\n");

	/*
	 * re-__malloc first pointer
	 */
	ptr[0] = __malloc(size);
	if(ptr[0] == NULL)
	{
		printf("re-malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}
	malloc_free_list();
	printf("\n");

	/*
	 * try splitting the second block
	 */
	ptr[1] = __malloc(size/2);
	if(ptr[1] == NULL)
	{
		printf("split second block ptr[1] failed for size %d\n",
				size/2);
		exit(1);
	}
	malloc_free_list();
	printf("\n");

	/*
	 * free first block and split of second
	 */
	__free(ptr[0]);
	__free(ptr[1]);

	malloc_free_list();
	printf("\n");

	/*
	 * try mallocing a little less to make sure no split occurs
	 * first block from previous print should not be split
	 */
	ptr[0] = __malloc(size-1);
	if(ptr[0] == NULL)
	{
		printf("slightly smaller malloc of ptr[0] failed for size %d\n",
				size);
		exit(1);
	}
	malloc_free_list();

	/*
	 * free it and make sure it comes back as the correct size
	 */
	__free(ptr[0]);
	
	malloc_free_list();
	printf("\n");

	/*
	 * okay, now see if multiples work
	 */
	for(i=0; i < 6; i++)
	{
		ptr[i] = __malloc(100);
	}

	/*
	 * free first block, third block, fifth block
	 */
	__free(ptr[0]);
	__free(ptr[2]);
	__free(ptr[4]);
	malloc_free_list();
	printf("\n");

	/*
	 * now, __free second block -- first, second, third blocks
	 * should coalesce
	 */
	__free(ptr[1]);

	malloc_free_list();
	printf("\n");

	/*
	 * free the sixth block and it should merge with the last
	 * block leaving two
	 */
	__free(ptr[5]);
	malloc_free_list();
	printf("\n");

	/*
	 * now __free fourth block and they should all be together
	 */
	__free(ptr[3]);
	malloc_free_list();
	printf("\n");

	printf("made it -- passed test\n");

	exit(0);
}

	

