#include "buddy.h"

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
    while((1<<order) * PAGE_SIZE <size && order<MAX_ORDER){
        order++;
    }
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
    init_page->order = MAX_ORDER-1;
    init_page->next = NULL;
    init_page->next->next = NULL;
    free_list[MAX_ORDER-1] = init_page;

}
void *allocate(unsigned int size) {
    uart_send_string("\n\r=========[allocate size] ");
    uart_send_int(size);
    uart_send_string("=========");
    int order=calculate_order(size);                //calculate the order of the allocated page


    int page_index=0;                               //page index of the allocated page
    page_t *page=NULL;                              //pointer to the allocated page
    int current_order=order;                        //current order to find the free list

    //From the order to MAX_ORDER, find a free list[current_order] which is enough to allocate the size
    while(current_order<MAX_ORDER && !page){
        if(free_list[current_order]!=NULL){         //check if the free list is not empty(already split the page)
            page=free_list[current_order];
            free_list[current_order]=page->next;    //remove the page from the free list[current_order] 
                                                    //ex: free_list[1]: [block@0] -> [block@2] -> NULL ===> free_list[1]: [block@2] -> NULL
            break;
        }
        current_order++;
    }


    if(!page){
        uart_send_string("\n\rNo enough memory");
        return NULL;
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
        for(int i=start_index+1;i<buddy_index;i++){
            free_array[i] = BELONGS_TO_LARGER;
        }
        free_array[buddy_index] = current_order-1;
        for(int i=buddy_index+1;i< buddy_index+(1<<(current_order-1));i++){
            free_array[i] = BELONGS_TO_LARGER;
        }
        

        //update free_list
        free_list[current_order] = free_list[current_order]->next;
        buddy-> next = NULL;
        buddy-> order = current_order-1;
        page->order = current_order-1;
        free_list[current_order-1] = page;
        page->next = buddy;


        //print the split page
        uart_send_string("\n\r[+] Add page ");
        uart_send_int(buddy_index);
        uart_send_string(" to current_order ");
        uart_send_int(current_order);
        uart_send_string(".");

        current_order--;
    }


    //calculate the page index of the allocated page
    for(int i=0;i<MAX_PAGES;i++){
        if(free_array[i]==order){
            page_index=i;
            break;
        }
    }

    //mark the page as allocated
    for(int i=0;i<(1<<order);i++){
        free_array[page_index+i]=ALLOCATED;
    }


    //update the order of the page
    page->order = order;
    free_list[order] = free_list[order]->next;
    
    //calculate the user pointer
    void* user_ptr = (void*)((unsigned int)page + sizeof(page_t));
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
    return user_ptr;  //return the user pointer
}
void free(void* ptr) {
    uart_send_string("\n\r=========[free] ");
    uart_send_hex(ptr);
    uart_send_string("=========");
    if (!ptr) return;
    
    //calculate the original page pointer
    void* page_ptr = (void*)((unsigned int)ptr - sizeof(page_t));
    uart_send_string("\n\r[Original page pointer] ");
    uart_send_hex(page_ptr);
    
    int index = calculate_page_index(page_ptr);
    page_t *page = (page_t *)page_ptr;
    
    // check if the memory is allocated
    if (free_array[index] != ALLOCATED) {
        uart_send_string("\n\rError: Trying to free unallocated memory\n\r");
        return;
    }
    
    // find the order of the block
    int order = page->order;
    
    // mark the block as free
    for(int i=index;i<index+(1<<order);i++){
        free_array[i]=BELONGS_TO_LARGER;
    }
    free_array[index]=order;
    // try to merge
    while (order < MAX_ORDER) {
        int buddy_index = index ^ (1<<order); //ex: index=0, order=1, buddy_index=1
        
        // check if the buddy is also free and the order is the same
        if (buddy_index >= MAX_PAGES || free_array[buddy_index] != order) {
            break;
        }
        
        // remove the buddy from the free list
        page_t* prev = NULL;
        page_t* curr = free_list[order];

        //find the buddy in the free list
        while (curr && calculate_page_index((void*)curr) != buddy_index) {
            prev = curr;
            curr = curr->next;
        }
        
        if (!curr) break;  // the buddy is not in the free list
        
        // remove the buddy
        if (prev) {                     //if current is not the first node
            prev->next = curr->next;
        } else {                        //if current is the first node
            free_list[order] = curr->next;
        }
        
        // merge.find the smaller index
        if (buddy_index < index) {
            index = buddy_index;
        }
        
        //update the order of the page
        order++;
        free_array[index] = order;
        // mark the buddy as belongs to a larger block
        for (int i = 0; i < (1 << (order - 1)); i++) {
            free_array[buddy_index + i] = BELONGS_TO_LARGER;
        }
        
        // print the merge log
        uart_send_string("\n\r[Merge] Merged page ");
        uart_send_int(index);
        uart_send_string(" with buddy ");
        uart_send_int(buddy_index);
        uart_send_string(" to form page ");
        uart_send_int(index);
        uart_send_string(" to order ");
        uart_send_int(order);
        uart_send_string(".");
    }
    
    // add the merged block to the free list
    page_t* block = (page_t*)calculate_address(index);
    block->next = free_list[order];
    block->order = order;
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
}

