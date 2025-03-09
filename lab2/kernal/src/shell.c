#include "shell.h"

char input_buffer[MAX_INPUT_LEN];
char filename_buffer[MAX_INPUT_LEN];
char size_str[MAX_INPUT_LEN];
int main() {
    // uart_init(); 
    uart_send_string("Hello, world!\n");
    init_allocator();
    while (1) { 
        uart_send_string("\n# ");

        input_string(input_buffer);

        if (strcmp(input_buffer, "help") == 0) {
            uart_send_string("\n\rhelp   :Show this message\n\r");
            uart_send_string("reboot :reboot the system\n\r");
            uart_send_string("ls     :list the files in the current directory\n\r");
            uart_send_string("cat    :show the content of a file\n\r");
            uart_send_string("malloc :allocate memory\n\r");
        } else if(strcmp(input_buffer, "reboot") == 0) {
            uart_send_string("\n\rReboot\n\r");
            reset(100);
        } else if(strcmp(input_buffer, "ls") == 0) {
            cpio_ls();
        } else if(strcmp(input_buffer, "cat") == 0) {
            uart_send_string("\n\rEnter filename: ");
            input_string(filename_buffer);
            uart_send_string("\n\r");
            cpio_cat(filename_buffer);
        } else if(strcmp(input_buffer, "malloc") == 0) {
            uart_send_string("\n\rEnter size: ");
            unsigned int size = input_int(size_str);
            uart_send_string("\n\r");
            void* ptr = simple_alloc(size);
            uart_send_string("Allocated memory at: ");
            uart_send_hex((unsigned int)ptr);
            uart_send_string("\n\r");
        } else {
            uart_send_string("\n\rInvalid command\n\r");
        }
    }
}