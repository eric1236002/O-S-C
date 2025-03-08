#include "shell.h"

char input_buffer[MAX_INPUT_LEN];

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int main() {
    // uart_init(); 
    uart_send_string("Hello, world!\n");
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
            uart_send_string("\n\rhelp    :Show this message\n\rreboot :reboot the system\r\nls    :list the files in the current directory\r");
        }else if(strcmp(input_buffer, "reboot")==0){
            uart_send_string("\n\rReboot\r");
            reset(100);
        }else if(strcmp(input_buffer, "ls")==0) {
            uart_send_string("\n\rListing files:\r");
            // list_files();
        }else {
            uart_send_string("\n\rInvalid command\r");
        }
    }
}