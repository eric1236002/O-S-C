#include "utils.h"

int hextodec(const char *hex) {
    int result = 0;
    
    // 處理所有 8 位數字
    for(int i = 0; i < 8; i++) {
        char c = hex[i];
        if (c >= '0' && c <= '9') {
            result = (result << 4) | (c - '0');
        } else if (c >= 'a' && c <= 'f') {
            result = (result << 4) | (c - 'a' + 10);
        } else if (c >= 'A' && c <= 'F') {
            result = (result << 4) | (c - 'A' + 10);
        }
    }
    
    return result;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}


void align(void *ptr, int alignment) {
    unsigned long *num = (unsigned long *)ptr;
    if(*num & ~(alignment - 1)){
        *num = (*num + alignment - 1) & ~(alignment - 1);
    }
}
    
void input_string(char *input_buffer) {
    int i = 0;
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
}

unsigned int input_int(char *input_buffer) {
    input_string(input_buffer);
    return my_atoi(input_buffer);
}

unsigned int my_atoi(const char *str) {
    unsigned int result = 0;
    while (*str) {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}