#include "shell.h"

char input_buffer[MAX_INPUT_LEN];

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
    uart_init();
    fdt_traverse(dtb_addr, initramfs_callback);
    while (1) { 
        int i = 0;
        uart_send_string("\n# ");

        while (1) {
            char c = uart_receive_char();
            if (c == '\n' || c == '\r') {
                break;
            }
            uart_send_char(c);
            if (i < MAX_INPUT_LEN - 1) {
                input_buffer[i++] = c;
            }
        }

        input_buffer[i] = '\0';

        if (strcmp(input_buffer, "help") == 0) {
            uart_send_string("\n\rhelp    :Show this message\n\rload   :use uart to load kernal\n\r");
        }else if(strcmp(input_buffer, "load")==0){
            uart_send_string("\r\ndtb_addr: ");
            uart_send_hex((unsigned long)dtb_addr);
            uart_send_string("\n\r");
            uart_send_string("shell initramfs_start: ");
            uart_send_hex((unsigned long)initramfs_start);
            uart_send_string("\n\r");
            uart_send_string("Bootloader\n");
            uart_send_string("\n\rLoad kernel\r");
            load_kernel();
        } else if(strcmp(input_buffer, "ls") == 0) {
            cpio_ls(initramfs_start);
        } else if(strcmp(input_buffer, "cat") == 0) {
            uart_send_string("\n\rEnter filename: ");
            input_string(filename_buffer);
            uart_send_string("\n\r");
            cpio_cat(filename_buffer,initramfs_start);
        }else {
            uart_send_string("\n\rInvalid command\r");
        }
    }
}