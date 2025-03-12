#include "shell.h"

char input_buffer[MAX_INPUT_LEN];



int main() {
    uart_init();
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
            load_kernel();
        }else {
            uart_send_string("\n\rInvalid command\r");
        }
    }
}