#include "buddy.h"

static mutex_t buddy_mutex = {0};

page_t *free_list[MAX_ORDER];
int free_array[MAX_PAGES];

void print_free_array(){
    uart_send_string("[");
    for(int i=0;i<MAX_PAGES;i++){
        uart_send_int(free_array[i]);
        uart_send_string(",");
    }
    uart_send_string("]");
}

void print_free_list(){
    for(int i=0;i<MAX_ORDER;i++){
        uart_send_string("\n\rfree_list[");
        uart_send_int(i);
        uart_send_string("] : ");
        page_t *current = free_list[i];
        while(current){
            uart_send_hex(current);
            uart_send_string(" -> ");
            current = current->next;
        }
        uart_send_string("NULL");
        uart_send_string(".");
    }
}

int calculate_order(unsigned int size){
    int order=0;
    while(order < MAX_ORDER && (1 << order) * PAGE_SIZE < size){
        order++;
    }
    if (order >= MAX_ORDER) 
        return -1;
    return order;
}


void *calculate_address(int index){
    return (void *)(MEM_START + index * PAGE_SIZE);
}

int calculate_page_index(page_t *page){
    //use the page address to calculate the page index
    return ((unsigned int)page - MEM_START) / PAGE_SIZE;
}


void buddy_init(){
    // initialize all free_list to NULL
    for(int i = 0; i < MAX_ORDER; i++) {
        free_list[i] = NULL;
    }
    
    // initialize free_array
    for(int i = 0; i < MAX_PAGES; i++) {
        free_array[i] = BELONGS_TO_LARGER;
    }
    free_array[0] = MAX_ORDER-1;

    // initialize the first page
    page_t *init_page = (page_t *)MEM_START;
    init_page->next = NULL;
    free_list[MAX_ORDER-1] = init_page;

    print_free_array();
    print_free_list();
    mutex_init(&buddy_mutex);

}
void *allocate_page(unsigned int size) {
    mutex_lock(&buddy_mutex);
    uart_send_string("\n\r=========[allocate size] ");
    uart_send_int(size);
    uart_send_string("=========");
    int order=calculate_order(size);                //calculate the order of the allocated page


    int page_index=0;                               //page index of the allocated page
    page_t *page=NULL;                              //pointer to the allocated page
    int current_order=order;                        //current order to find the free list

    while (current_order < MAX_ORDER) {
        if (free_list[current_order] != NULL) {
            page = free_list[current_order];
            break;
        }
        current_order++;
    }


    if(!page){
        uart_send_string("\n\rNo enough memory");
        mutex_unlock(&buddy_mutex);
        return NULL;
    }
    
    free_list[current_order] = page->next;
    if (free_list[current_order]) {
        free_list[current_order]->prev = NULL;
    }


    //check if the current order is larger than the order.
    //split the page ? larger than the order : equal to the order
    while(current_order>order){

        //calculate the split index
        unsigned int start_index = calculate_page_index(page);

        //get the split index address
        page_t* buddy = (page_t*)calculate_address(start_index + (1<<(current_order-1)));
        int buddy_index = start_index + (1<<(current_order-1));
        

        //update free_array
        free_array[start_index] = current_order-1;
        free_array[buddy_index] = current_order-1;

        

        //update free_list
        int lower_order = current_order - 1;
        page_t* lower_list_head = free_list[lower_order];

        buddy->prev = NULL;
        buddy->next = lower_list_head; 
        if (lower_list_head) {      
            lower_list_head->prev = buddy;
        }
        free_list[lower_order] = buddy;

        //print the split page
        uart_send_string("\n\r[+] Add page ");
        uart_send_int(buddy_index);
        uart_send_string(" to current_order ");
        uart_send_int(current_order);
        uart_send_string(".");

        current_order--;
    }


    //calculate the page index of the allocated page
    page_index = calculate_page_index(page);


    free_array[page_index]=order-100;

    
    //calculate the user pointer
    void* user_ptr = (void*)(unsigned int)page;
    //==============debug==============
    uart_send_string("\n\r[after allocate]");
    print_free_array();
    print_free_list();
    //==============debug==============
    uart_send_string("\n\r[Page] Allocate ");
    uart_send_hex(page);
    uart_send_string(" (user: ");
    uart_send_hex(user_ptr);
    uart_send_string(") at order ");
    uart_send_int(order);
    uart_send_string(", page index ");
    uart_send_int(page_index);
    mutex_unlock(&buddy_mutex);
    return user_ptr;  //return the user pointer
}
void free_page(void* ptr) {
    mutex_lock(&buddy_mutex);
    uart_send_string("\n\r=========[free] ");
    uart_send_hex(ptr);
    uart_send_string("=========");
    if (!ptr) {
        mutex_unlock(&buddy_mutex);
        return;
    }
    
    //calculate the original page pointer
    void* page_ptr = (void*)(unsigned int)ptr;
    uart_send_string("\n\r[Original page pointer] ");
    uart_send_hex(page_ptr);
    
    int index = calculate_page_index(page_ptr);
    page_t *page = (page_t *)page_ptr;
    
    // check if the memory is allocated
    if (free_array[index] >-2) {
        uart_send_string("\n\rError: Trying to free unallocated memory\n\r");
        mutex_unlock(&buddy_mutex);
        return;
    }
    
    // find the order of the block
    int order = free_array[index]+100;
    

    free_array[index]=order;
    // try to merge
    while (order < MAX_ORDER) {
        int buddy_index = index ^ (1<<order); //ex: index=0, order=1, buddy_index=1
        
        // check if the buddy is also free and the order is the same
        if (buddy_index >= MAX_PAGES || free_array[buddy_index] != order) {
            break;
        }
        
        // remove the buddy from the free list
        page_t* buddy_page = (page_t*)calculate_address(buddy_index);
        if (!buddy_page) break;

        // From the buddy to the free list
        if (buddy_page->prev) { // Case 1: buddy is not head
            buddy_page->prev->next = buddy_page->next;
        } else { // Case 2: buddy is head
            free_list[order] = buddy_page->next;
        }
        if (buddy_page->next) { // Case 3: buddy is not tail
            buddy_page->next->prev = buddy_page->prev;
        }

        
        // merge.find the smaller index
        int merged_index = (index < buddy_index) ? index : buddy_index;
        int old_buddy_index = (index < buddy_index) ? buddy_index : index;
        
        order++; //update the order
        // update the free_array
        free_array[merged_index] = order;
        free_array[old_buddy_index] = BELONGS_TO_LARGER;
        index = merged_index;

        
        // print the merge log
        uart_send_string("\n\r[Merge] Merged page ");
        uart_send_int(index);
        uart_send_string(" with buddy ");
        uart_send_int(buddy_index);
        uart_send_string(" . From page ");
        uart_send_int(index);
        uart_send_string(" to order ");
        uart_send_int(order);
        uart_send_string(".");
    }
    
    // add the merged block to the free list
    page_t* block = (page_t*)calculate_address(index);
    page_t* old_head = free_list[order];
    block->prev = NULL;
    block->next = old_head;
    if (old_head) {
        old_head->prev = block;
    }
    free_list[order] = block;

    //==============debug==============
    uart_send_string("\n\r[after allocate]");
    print_free_array();
    print_free_list();
    //==============debug==============
    
    // print the free log
    uart_send_string("\n\r[-] Remove page index ");
    uart_send_int(index);
    uart_send_string(" from order ");
    uart_send_int(order);
    uart_send_string(".");
    mutex_unlock(&buddy_mutex);
}
void memory_reserve(void *start, void *end) {
    uart_send_string("\n\r=========[memory_reserve] ");
    uart_send_hex(start);
    uart_send_string(" - ");
    uart_send_hex(end);
    uart_send_string("=========");

    if (start < (void *)MEM_START) 
        start = (void *)MEM_START;
    if (end > (void *)MEM_END) 
        end = (void *)MEM_END;

    if(end <= MEM_START){
        uart_send_string("\n\r[Warning] end <= MEM_START");
        return;
    }
    if(start >= MEM_END){
        uart_send_string("\n\r[Warning] start >= MEM_END");
        return;
    }

    unsigned int start_addr = (unsigned int)start;
    unsigned int end_addr = (unsigned int)end;

    unsigned int aligned_start = (start_addr / PAGE_SIZE) * PAGE_SIZE;
    unsigned int num = (end_addr + PAGE_SIZE - 1) / PAGE_SIZE;
    if(num==0){
        num=1;
    }
    unsigned int aligned_end = num * PAGE_SIZE;

    uart_send_string("\n\r[Aligned] ");
    uart_send_hex(aligned_start);
    uart_send_string(" - "); 
    uart_send_hex(aligned_end);
    
    unsigned int num_pages = (aligned_end - aligned_start) / PAGE_SIZE;
    
    if (num_pages == 0) {
        uart_send_string("\n\r[Warning] No pages to reserve");
        return;
    }
    
    uart_send_string("\n\r[Reserving] ");
    uart_send_int(num_pages);
    uart_send_string(" pages");


    unsigned int ptr_addr[1<<MAX_ORDER];
    int i = 0;

    for (unsigned int addr = MEM_START; addr < aligned_end;) {
        void *temp=allocate_page(PAGE_SIZE);
        if (temp == NULL) {
            uart_send_string("\n\r[Error] Failed to allocate pages");
            mutex_unlock(&buddy_mutex);
            return;
        }
        
        if ((unsigned int)temp < aligned_start || (unsigned int)temp >= aligned_end) {
            ptr_addr[i] = (unsigned int)temp;
            i++;
        } else {
            uart_send_string("\n\r[Reserved] Page at ");
            uart_send_hex((unsigned int)temp);
        }
        addr=(unsigned int)temp;
    }

    for (int j = 0; j < i; j++) {
        free_page((void *)ptr_addr[j]);
    }
    mutex_unlock(&buddy_mutex);
    uart_send_string("\n\r[Memory reservation completed]");
}