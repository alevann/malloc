#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#define MAX_MALLOC_SIZE (1024*1024*16)

void init_malloc();
void* __malloc(int);
void __free(void*);
void coalesce_free_blocks();

void malloc_free_list();


#endif