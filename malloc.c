#include "malloc.h"
#include <stdio.h>

#define mheaderptr struct malloc_hdr*
#define mhs sizeof(struct malloc_hdr)

// A 32 bytes sized header containing data required for book-keeping
// The pointers next & prev are only used in the free function 
// and should not point to reserved allocated blocks. 
// The size does NOT include the size of the header, it is the exact size of the buffer.
struct malloc_hdr {
  struct malloc_hdr* next;  // previous header
  struct malloc_hdr* prev;  // next header
  int size;                 // size of the buffer
  unsigned char* buffer;    // start of the buffer
};

unsigned char main_buffer[MAX_MALLOC_SIZE];   // The preallocated buffer holding the data
mheaderptr hfree;                             // The head of the linked list holding the free blocks

// Initializes the genesis block at the start of the main_buffer
// the size of the entirety of the buffer.
void init_malloc()
{
  hfree = (void*) &main_buffer[0];
  hfree->buffer = (unsigned char*)(hfree + 1);
  hfree->next = NULL;
  hfree->prev = NULL;
  hfree->size = MAX_MALLOC_SIZE - mhs;

  printf("Genesis at: 0x%p\n", hfree);
}

void* __malloc(int size)
{
  // No more avilable memory is left
  if (hfree == NULL) return NULL;

  int hblocksize = size + mhs;    // Size of the header and allocated block
  mheaderptr cur = hfree;

  do {
    // Not enough space in this block
    if (cur->size < size) {
      continue;
    }
    
    // There is enough space but the header won't fit
    if (cur->size >= size && cur->size < hblocksize)
    {
      // The current block will hijacked so
      // make sure the head points to a valid block
      if (cur == hfree) hfree = cur->next;

      // Remove the hijacked block from the free list
      cur->next->prev = cur->prev;
      cur->prev->next = cur->next;

      cur->next = NULL;
      cur->prev = NULL;

      return cur->buffer;
    }
    else
    {
      // Here we have more than enough space for both the header and the buffer
      // The current header will be hijacked, the buffer shortened, 
      // and a new header will be created just at the end of the block
      mheaderptr nhdr = (mheaderptr)(cur->buffer + size);
      nhdr->next = cur->next;
      nhdr->prev = cur->prev;
      nhdr->size = cur->size - hblocksize;
      nhdr->buffer = (unsigned char*)(nhdr + 1);

      // Any prev and next pointers need to be kept updated
      // since the cur has been replaced by the new header
      if (nhdr->next) nhdr->next->prev = nhdr;
      if (nhdr->prev) nhdr->prev->next = nhdr;

      cur->next = NULL;
      cur->prev = NULL;

      // The difference between the beginning of the next header and 
      // the start of the buffer, is the size the buffer has been shrinked to
      cur->size = ((unsigned char*) nhdr) - cur->buffer;

      // If the current node was the head, the new node is now the head
      if (cur == hfree) hfree = nhdr;

      return cur->buffer;
    }
  } while ((cur = cur->next));

  // The allocation fails because there's not enough contiguous space for it
  // Free blocks are merged after each call to free, trying that here is useless
  printf("Allocation of %d has failed.\n", size);
  return NULL;
}

void __free(void* buffer)
{
  mheaderptr tfree = (mheaderptr)(((unsigned char*) buffer) - mhs);

  if (hfree == NULL)
  {
    // The block is now the only free block in the list
    tfree->next = NULL;
    tfree->prev = NULL;
    hfree = tfree;
  }
  else if (hfree < tfree)
  {
    // The block should not become the head fo the list,
    // so we must find where the block belongs in the list
    mheaderptr cur = hfree->next;

    do
    {
      // If the current block is before the block we can skip it
      // the current block needs to be after the block to free
      if (cur < tfree)
        continue;
      
      // The current block must become the next block after the one to free
      // Update the node to free to point to the right blocks in the list
      tfree->next = cur;
      tfree->prev = cur->prev;

      // Update the other blocks so they point to the jsut freed header
      tfree->prev->next = tfree;
      tfree->next->prev = tfree;

      goto coalesce;  // am i evilll??? yes i am -- am i eviiilll?? i am maan - yes i ammm

    } while ((cur = cur->next));

    // If the last value's next is null, 
    // the block to free is at the end of the list
    if (cur->next == NULL)
    {
      tfree->next = NULL;
      tfree->prev = cur;
      cur->next = tfree;
    }
  }
  else
  {
    // Here the block to free must become the head of the list
    tfree->next = hfree;  // next becomes the current head of the list
    tfree->prev = NULL;   // no previous node
    hfree->prev = tfree;  // the current head must point to this block
    hfree = tfree;
  }

  coalesce:
  coalesce_free_blocks();
}

// Merges together free blocks that are next to one another
void coalesce_free_blocks()
{
  mheaderptr cur = hfree;

  do {
    mheaderptr next = (void*)(cur->buffer + cur->size);

    // Keep merging blocks together as long as they're next to one another
    while (1)
    {
      // If the next free block is not immediately at the end 
      // of the current block, no more merging can't happen
      if (next != cur->next)
        break;
      
      // Update the current node to include the 
      // next blocks's buffer and header
      cur->size += next->size + mhs;
      cur->next = next->next;

      // Make sure the next free block points to the current block
      // and not the block just merged
      if (next->next) next->next->prev = cur;

      next = (void*)(cur->buffer + cur->size);
    }
  } while ((cur = cur->next));
}

void malloc_free_list()
{
  struct malloc_hdr* cur = hfree;
  do {
    printf("block: 0x%p\n", cur);
    printf("\tsize: 0x%d\n", cur->size);
    printf("\tnext: 0x%p\n", cur->next);
    printf("\tprev: 0x%p\n", cur->prev);
    printf("\tbuffer: 0x%p\n", cur->buffer);
  } while ((cur = cur->next) != 0x0);
}