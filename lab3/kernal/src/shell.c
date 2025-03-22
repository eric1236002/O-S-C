#include "shell.h"

char input_buffer[MAX_INPUT_LEN];
char filename_buffer[MAX_INPUT_LEN];
char size_str[MAX_INPUT_LEN];

void initramfs_callback(const char *node_name, const char *property_name, const void *property_value)
{
    if(strcmp(property_name,"linux,initrd-start")==0){
        initramfs_start = bswap32(*(unsigned long *)property_value);
        uart_send_string("\n\rInitramfs start: ");
        uart_send_hex(initramfs_start);
        return;
    }
}

int main() {
    // uart_init(); 
    uart_send_string("\n\rHello, world!");
    init_allocator();
    uart_send_string("\n\rdtb_addr: ");
    uart_send_hex((unsigned long)dtb_addr);
    uart_send_string("\n");

    // 檢查 DTB 頭部的前幾個字節
    uart_send_string("\n\rDTB header bytes: ");
    unsigned char* dtb_bytes = (unsigned char*)dtb_addr;
    for (int i = 0; i < 16; i++) {
        uart_send_hex(dtb_bytes[i]);
        uart_send_string(" ");
    }
    uart_send_string("\n");

    fdt_traverse(dtb_addr,initramfs_callback);

    unsigned int el = get_el();
    print_el();
    while (1) { 
        uart_send_string("\n\r# ");

        input_string(input_buffer);

        if (strcmp(input_buffer, "help") == 0) {
            uart_send_string("\n\rhelp   :Show this message\n\r");
            uart_send_string("reboot :reboot the system\n\r");
            uart_send_string("ls     :list the files in the current directory\n\r");
            uart_send_string("cat    :show the content of a file\n\r");
            uart_send_string("user :load program to memory\n\r");
            uart_send_string("malloc :allocate memory\n\r");
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
        } else if(strcmp(input_buffer, "malloc") == 0) {
            uart_send_string("\n\rEnter size: ");
            unsigned int size = input_int(size_str);
            uart_send_string("\n\r");
            void* ptr = simple_alloc(size);
            uart_send_string("Allocated memory at: ");
            uart_send_hex64((unsigned long)ptr);
            uart_send_string("\n\r");
        } 
        else if(strcmp(input_buffer, "user") == 0) {
            uart_send_string("\n\rEnter filename: ");
            input_string(filename_buffer);
            uart_send_string("\n\r");
            cpio_load_program(filename_buffer, initramfs_start);
        }
        else {
            uart_send_string("\n\rInvalid command\n\r");
        }
    }
}