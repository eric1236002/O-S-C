#define NUM_POOLS 8
#define MAX_SIZE 2048
#include "buddy.h"
const int pool_sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048};


typedef struct slab_page_t {
    int pool_index;           // used to find the pool
    struct slab_page_t *next;
} slab_page_t;

typedef struct chunk_t {
    struct chunk_t *next;
} chunk_t;

typedef struct pool_t { 
    int count;
    chunk_t *free_list;       // the chunk linked list
    slab_page_t *pages;           // the page list
} pool_t;

extern pool_t pools[NUM_POOLS];


void init_pools();
void *malloc(unsigned int size);
void free(void *ptr);
