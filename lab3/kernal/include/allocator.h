#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stddef.h>
#define MEM_SIZE 1024*1024// 1 mb
#define MEM_START 0x10000000

void init_allocator();
void* simple_alloc(size_t size);

#endif


