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
    uart_init(); 
    uart_send_string("Bootloader\n");
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
            uart_send_string("\n\rhelp    :Show this message\n\rload   :use uart to load kernal\n\rreboot :reboot the system\r");
        }else if(strcmp(input_buffer, "load")==0){
            uart_send_string("\n\rLoad kernel\r");
            load_kernel();
        }else if(strcmp(input_buffer, "reboot")==0){
            uart_send_string("\n\rReboot\r");
            reset(100);
        }else {
            uart_send_string("\n\rInvalid command\r");
        }
    }
}