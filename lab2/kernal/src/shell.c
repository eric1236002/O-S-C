#include "shell.h"

char input_buffer[MAX_INPUT_LEN];
char filename_buffer[MAX_INPUT_LEN];

int main() {
    // uart_init(); 
    uart_send_string("Hello, world!\n");
    while (1) { 
        uart_send_string("\n# ");

        input_string(input_buffer);

        if (strcmp(input_buffer, "help") == 0) {
            uart_send_string("\n\rhelp   :Show this message\n\r");
            uart_send_string("reboot :reboot the system\n\r");
            uart_send_string("ls     :list the files in the current directory\n\r");
            uart_send_string("cat    :show the content of a file\n\r");
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
        } else {
            uart_send_string("\n\rInvalid command\n\r");
        }
    }
}