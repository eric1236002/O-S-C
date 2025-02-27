
#include <stdint.h>

#define PBASE 0x3F000000
#define AUX_BASE (PBASE + 0x00215000)
#define AUX_MU_IO_REG  ((volatile uint32_t*)(AUX_BASE + 0x40))
#define AUX_MU_LSR_REG ((volatile uint32_t*)(AUX_BASE + 0x54))
#define MAX_INPUT_LEN 100
char input_buffer[MAX_INPUT_LEN];

void uart_send_string(const char *str) {
    while (*str) {
        while (!(*AUX_MU_LSR_REG & 0x20)) {} // 等待 UART 可用
        *AUX_MU_IO_REG = (uint32_t)(*str++);
    }
}

char uart_receive_char() {
    while (!(*AUX_MU_LSR_REG & 0x01)) {} // 等待數據準備好
    return (char)(*AUX_MU_IO_REG & 0xFF);
}


int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int main() {
    while (1) { 
        int i = 0;
        uart_send_string("Welcome to OSC Lab1\n");
        uart_send_string("\n# ");

        while (1) {
            char c = uart_receive_char();
            if (c == '\n' || c == '\r') {
                break;
            }
            if (i < MAX_INPUT_LEN - 1) {
                input_buffer[i++] = c;
            }
        }

        input_buffer[i] = '\0'; // 確保字符串結束

        if (strcmp(input_buffer, "hello") == 0) {
            uart_send_string("Hello, World!\n");
        } else if (strcmp(input_buffer, "help") == 0) {
            uart_send_string("Available commands:\nhello - Print greeting\nhelp - Show this message\n");
        } else {
            uart_send_string("Invalid command\n");
        }
    }
}
