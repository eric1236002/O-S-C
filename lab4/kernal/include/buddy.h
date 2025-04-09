#ifndef BUDDY_H_
#define BUDDY_H_
#include "uart.h"

#define MAX_ORDER 4               // because kernal usually manage 8MB 
#define MEM_START 0x07000000        // 256MB
#define MEM_END MEM_START+(1<<MAX_ORDER-1)*PAGE_SIZE          // 512MB
#define PAGE_SIZE 4096
#define MAX_PAGES (1<<MAX_ORDER-1)
#define NULL ((void*)0)


#define ALLOCATED -1                // <X> represents allocated
#define BELONGS_TO_LARGER -2        // <F> represents belongs to larger
typedef struct page {
    struct page *prev;
    struct page* next;
} page_t;


extern page_t *free_list[MAX_ORDER];
extern int free_array[MAX_PAGES];      // number of free pages. The size of free_array is 2^MAX_ORDER

void *allocate_page(unsigned int size);
void free_page(void *ptr);
void buddy_init();
void memory_reserve(void *start, void *end);
#endif
