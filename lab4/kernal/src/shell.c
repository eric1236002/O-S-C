#include "shell.h"

char input_buffer[MAX_INPUT_LEN];
char filename_buffer[MAX_INPUT_LEN];
char size_str[MAX_INPUT_LEN];
char message_buffer[MAX_INPUT_LEN];

void initramfs_callback(const char *node_name, const char *property_name, const void *property_value)
{
    if(strcmp(property_name,"linux,initrd-start")==0){
        initramfs_start = bswap32(*(unsigned long *)property_value);
        uart_send_string("\n\rInitramfs start: ");
        uart_send_hex(initramfs_start);

    }
    if(strcmp(property_name,"linux,initrd-end")==0){
        initramfs_end = bswap32(*(unsigned long *)property_value);
        uart_send_string("\n\rInitramfs end: ");
        uart_send_hex(initramfs_end);
    }
    return;
}

int main() {
    // uart_init(); 
    uart_send_string("\n\rHello, world!");
    init_allocator();
    uart_send_string("\n\rdtb_addr: ");
    uart_send_hex((unsigned long)dtb_addr);
    uart_send_string("\n");

    enable_interrupt();  
    uart_send_string("\n\rDTB header bytes: ");
    unsigned char* dtb_bytes = (unsigned char*)dtb_addr;
    for (int i = 0; i < 16; i++) {
        uart_send_hex(dtb_bytes[i]);
        uart_send_string(" ");
    }
    uart_send_string("\n");

    unsigned int el = get_el();
    print_el();
    buddy_init();

    uart_send_string("\n\r[OS] Pre-allocating critical memory regions...\n\r");

    uart_send_string("[OS] Reserved kernel code region\n\r");
    extern char _start, _end;
    memory_reserve(&_start, &_end);



    uart_send_string("\n\r[OS] Reserved initramfs region\n\r");
    fdt_traverse(dtb_addr, initramfs_callback);
    cpio_init(initramfs_start,initramfs_end);
    

    uart_send_string("\n\r[OS] Reserved kernel heap region");
    memory_reserve((void*)0x10000000, (void*)0x10010000);


    init_pools();
    
    while (1) { 
        uart_send_string("\n\r# ");

        input_string(input_buffer);

        if (strcmp(input_buffer, "help") == 0) {
            uart_send_string("\n\rhelp   :Show this message\n\r");
            uart_send_string("reboot :reboot the system\n\r");
            uart_send_string("ls     :list the files in the current directory\n\r");
            uart_send_string("cat    :show the content of a file\n\r");
            uart_send_string("exec   :load program to memory\n\r");
            uart_send_string("malloc :allocate memory\n\r");
            uart_send_string("timer  :set timer to send message\n\r");
            uart_send_string("alloc  :test allocate memory\n\r");
        } else if(strcmp(input_buffer, "reboot") == 0) {
            uart_send_string("\n\rReboot\n\r");
            reset(100);
        } else if(strcmp(input_buffer, "ls") == 0) {
            cpio_ls(initramfs_start);
        } else if(strcmp(input_buffer, "cat") == 0) {
            uart_send_string("\n\rEnter filename: ");
            input_string(filename_buffer);
            uart_send_string("\n\r");
            cpio_cat(filename_buffer,initramfs_start);
        } 
        else if(strcmp(input_buffer, "malloc") == 0) {
            uart_send_string("\n\rEnter size: ");
            unsigned int size = input_int(size_str);
            uart_send_string("\n\r");
            void* ptr = malloc(size);
            uart_send_string("\n\rAllocated memory at: ");
            uart_send_hex((unsigned long)ptr);
            uart_send_string("\n\r");
        } 
        else if(strcmp(input_buffer, "free") == 0) {
            uart_send_string("\n\rEnter pointer: ");
            char hex_buffer[MAX_INPUT_LEN];
            input_string(hex_buffer);
            
            unsigned int address = hextodec(hex_buffer);
            void* ptr = (void*)address;
            
            uart_send_string("\n\rFreeing address: ");
            uart_send_hex((unsigned long)ptr);
            uart_send_string("\n\r");
            free(ptr);
        }
        else if(strcmp(input_buffer, "exec") == 0) {
            uart_send_string("\n\rEnter filename: ");
            input_string(filename_buffer);
            uart_send_string("\n\r");
            cpio_load_program(filename_buffer, initramfs_start);
        }
        else if(strcmp(input_buffer, "timer") == 0) {
            uart_send_string("\n\rEnter time: ");
            unsigned int time = input_int(size_str);
            uart_send_string("\n\rEnter message: ");
            input_string(message_buffer);
            uart_send_string("\n\r");
            setTimeout(message_buffer, time);
        }
        else if(strcmp(input_buffer, "rs") == 0) {
            memory_reserve((void*)0x08000000, (void*)0x08100000);
        }
        else if(strcmp(input_buffer, "pp") == 0) {
            print_pools();
        }
        else if(strcmp(input_buffer, "pfa") == 0) {
            uart_send_string("\n\r");
            uart_send_string("free_array: ");
            print_free_array();
        }
        else if(strcmp(input_buffer, "pfl") == 0) {
            uart_send_string("\n\r");
            uart_send_string("free_list: ");
            print_free_list();
        }
        else {
            uart_send_string("\n\rInvalid command\n\r");
        }
    }
}