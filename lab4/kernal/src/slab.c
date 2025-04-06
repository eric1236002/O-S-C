#include "slab.h"

// mutex for protecting buffer
static mutex_t slab_mutex = {0};

pool_t pools[NUM_POOLS];
void print_pools(){
    uart_send_string("\n\r[pools]");
    for(int i=0;i<NUM_POOLS;i++){
        uart_send_int(i);
        uart_send_string(" : ");
        uart_send_int(pools[i].count);
        uart_send_string(" : ");
        uart_send_hex((unsigned long)pools[i].free_list-pool_sizes[i]-sizeof(chunk_t));
        uart_send_string(" : ");
        uart_send_hex((unsigned long)pools[i].pages);
        uart_send_string("\n\r");
    }
}
int find_pool(unsigned int size) {
    int left = 0;
    int right = NUM_POOLS - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        if (size <= pool_sizes[mid]) {
            if (mid == 0 || size > pool_sizes[mid - 1]) 
                return mid;
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return -1;
}

void init_pools() {
    for (int i = 0; i < NUM_POOLS; i++) {
        pools[i].count = 0;
        pools[i].free_list = NULL;
        pools[i].pages = NULL;
    }
    mutex_init(&slab_mutex);
}

void *malloc(unsigned int size) {
    mutex_lock(&slab_mutex);
    uart_send_string("\n\r=...===...===..malloc ");
    uart_send_int(size);
    uart_send_string("=...===...===..\n\r");
    if (size > MAX_SIZE) {
        uart_send_string("\n\rfallback to buddy allocator (size > MAX_SIZE)");
        mutex_unlock(&slab_mutex);
        return allocate_page(size);  // fallback to buddy allocator
    }
    int pool_index = find_pool(size);
    //==========debug==========
    uart_send_string("\n\rsize: ");
    uart_send_int(size);
    uart_send_string("\n\rpool_index: ");
    uart_send_int(pool_index);
    uart_send_string("\n\rpool_sizes: ");
    uart_send_int(pool_sizes[pool_index]);
    uart_send_string("\n\r");
    //==========debug==========

    if (pool_index == -1) {
        uart_send_string("\n\rfallback to buddy allocator (no suitable pool)");
        mutex_unlock(&slab_mutex);
        return allocate_page(size);  // fallback to buddy allocator
    }

    pool_t *pool = &pools[pool_index];

    // if the free list is empty, allocate a new page
    if (pool->free_list == NULL) {
        uart_send_string("\n\rallocate a new page");
        slab_page_t *new_page = (slab_page_t *)allocate_page(PAGE_SIZE);
        if (!new_page) {
            uart_send_string("\n\rfailed to allocate page");
            return NULL;
        }

        // mark this page belongs to which pool
        new_page->pool_index = pool_index;
        new_page->next = NULL;
        
        // add the new page to the pool's page list
        if (pool->pages == NULL) {
            pool->pages = new_page;
        } else {
            // add to the head of the list, so it's easier to find
            new_page->next = pool->pages;
            pool->pages = new_page;
        }

        int chunk_size = pool_sizes[pool_index];
        int real_chunk_size = chunk_size + sizeof(chunk_t);
        int num_chunks = (PAGE_SIZE - sizeof(slab_page_t)) / real_chunk_size;

        char *chunk_base = (char *)new_page + sizeof(slab_page_t);
        chunk_t *prev = NULL;

        for (int i = num_chunks - 1; i >= 0; i--) {
            chunk_t *chunk = (chunk_t *)(chunk_base + i * real_chunk_size);
            chunk->next = prev;
            prev = chunk;
        }

        pool->free_list = prev;
    }

    // allocate a chunk from the free list
    chunk_t *chunk = pool->free_list;
    pool->free_list = chunk->next;
    pool->count++;
    uart_send_string("\n\rchunk size: ");
    uart_send_int(sizeof(chunk_t));
    uart_send_string("\n\rallocate a chunk from the free list :");
    uart_send_hex((unsigned long)chunk);
    uart_send_string("\n\r");
    mutex_unlock(&slab_mutex);
    return (void *)chunk;
}

void free(void *ptr) {
    //==========debug==========
    uart_send_string("\n\r=~~~~~~~~~=free a chunk ");
    uart_send_hex((unsigned long)ptr);
    uart_send_string("=~~~~~~~~~=\n\r");
    //==========debug==========
    if (!ptr) {
        uart_send_string("\n\rInvalid page pointer");
        return;
    }

    mutex_lock(&slab_mutex);
    // check if the pointer is in the valid memory range
    if ((unsigned int)ptr < MEM_START || (unsigned int)ptr >= MEM_END) {
        uart_send_string("\n\rInvalid pointer - out of memory range");
        mutex_unlock(&slab_mutex);
        return;
    }

    unsigned int address = (unsigned int)ptr;
    unsigned int page_addr = address & ~(PAGE_SIZE-1);
    
    // check if the address is page aligned
    if (address == page_addr) {
        uart_send_string("\n\rfallback to buddy free (page aligned address)");
        free_page(ptr);
        mutex_unlock(&slab_mutex);
        return;
    }
    
    // try to interpret the page start address as slab_page_t
    slab_page_t *page = (slab_page_t *)page_addr;
    if (!page) {
        uart_send_string("\n\rInvalid page pointer");
        mutex_unlock(&slab_mutex);
        return;
    }
    
    unsigned int pool_index = page->pool_index;


    // if the pool_index is out of range, it means this is allocated by buddy allocator
    if (page->pool_index >= NUM_POOLS || pools[pool_index].count == 0) {
        uart_send_string("\n\rfallback to buddy free");
        mutex_unlock(&slab_mutex);
        free_page(ptr);
        return;
    }

    pool_t *pool = &pools[pool_index];

    
    // put the chunk back to the free list
    chunk_t *chunk = (chunk_t *)ptr;
    chunk->next = pool->free_list;
    pool->free_list = chunk;
    pool->count--;
    

    
    //if the pool is empty, free the page
    if (pool->count == 0) {
        uart_send_string("\n\r========>Go to free the page");
        // remove the page from the pool's page list
        if (pool->pages == page) {
            pool->pages = page->next;
        } else {
            slab_page_t *current = pool->pages;
            while (current && current->next != page) {
                current = current->next;
            }
            if (current) {
                current->next = page->next;
            } else {
                uart_send_string("\n\rWarning: Page not found in pool's page list");
                mutex_unlock(&slab_mutex);
                return;
            }
        }
        pool->free_list = NULL;
        
        mutex_unlock(&slab_mutex);
        free_page((void *)page);
        return;
    }
    uart_send_string("\n\r[After free] Pool count: ");
    uart_send_int(pool->count);
    mutex_unlock(&slab_mutex);
}

