#include "allocator.h"

static char *heap_top = (char *)MEM_START;  // 記錄 Heap 目前的可用位置
static char *heap_end = (char *)(MEM_START + MEM_SIZE);  // Heap 的最大範圍

void init_allocator() {
    // initialize the allocator
    heap_top = (char *)MEM_START;
    heap_end = (char *)(MEM_START + MEM_SIZE);
}

void* simple_alloc(size_t size) {
    // align the heap_top to 8 bytes
    heap_top = (char *)(((unsigned long)heap_top + 7) & ~7);
    
    // align the size to 8 bytes
    size = (size + 7) & ~7;
    
    if(heap_top + size > heap_end)
        return NULL;
        
    void* ret = heap_top;
    heap_top += size;
    return ret;
}
