#include "utils.h"

int hextodec(const char *hex) {
    int result = 0;
    int i = 0;
    
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


char* itoa(int num, int base) {
    int i = 0;
    char str[32];
    if (num == 0) {
        str[i++] = '0'; 
        str[i] = '\0';
        return str;
    }
    if(base == 16){
        str[i++] = '0';
        str[i++] = 'x';
    }
    while(num){
        int rem= num%base;
        if(rem>9){
            str[i++]=rem-10+'a';
        }
        else{
            str[i++]=rem+'0';
        }
        num/=base;
    }
    str[i]='\0';
    return str;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

    
void input_string(char *input_buffer) {
    int i = 0;
    while (1) {
        char c = uart_receive_char();
        if (c == '\n' || c == '\r') {
            break;
        }
        
        if (c == '\b' || c == 0x7F) {
            if (i > 0) {
                i--;
                uart_send_char('\b');
                uart_send_char(' ');
                uart_send_char('\b');
            }
        } else {
            uart_send_char(c);
            if (i < MAX_INPUT_LEN - 1) {
                input_buffer[i++] = c;
            }
        }
    }
    input_buffer[i] = '\0';
}

unsigned int input_int(char *input_buffer) {
    input_string(input_buffer);
    return my_atoi(input_buffer);
}

char* input_hex(char *input_buffer) {
    input_string(input_buffer);
    return input_buffer;
}

unsigned int my_atoi(const char *str) {
    unsigned int result = 0;
    while (*str) {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

unsigned long align(unsigned long num, unsigned long alignment) {
    return (num + alignment - 1) & ~(alignment - 1);

}

unsigned int string_len(const char *str) {
    unsigned int len = 0;
    while (*str++) {
        len++;
    }
    return len;
}

char *utohx(unsigned int num) {
    char str[9];
    str[9] = '\0';
    int i = 7;
    while (num > 0) {
        if(num % 16 > 9){
            str[i--] = num % 16 - 10 + 'a';
        }
        else{
            str[i--] = num % 16 + '0';
        }
        num /= 16;
    }
    return str;
}

void print_el() {
    unsigned int el;

    asm volatile("mrs %0, CurrentEL" : "=r"(el));
    el = (el >> 2) & 0x3;  
    
    uart_send_string("\n\rException level: ");
    uart_send_int(el);
    uart_send_string("\n\r");
}

char* strcpy(char* dest, const char* src){
    char* original_dest=dest;
    while(*src!='\0'){
        *dest++=*src++;
    }
    *dest='\0';
    return original_dest;
}

// initialize mutex
void mutex_init(mutex_t *mutex) {
    mutex->locked = 0;
}

// get mutex
void mutex_lock(mutex_t *mutex) {
    disable_interrupt();
    // uart_send_string("\n\rmutex locked");
    while(mutex->locked) {
        // if mutex is locked, release interrupt and try again
        enable_interrupt();
        // simple delay
        for(volatile int i = 0; i < 100; i++);
        disable_interrupt();
    }
    mutex->locked = 1;
    enable_interrupt();
}

// release mutex
void mutex_unlock(mutex_t *mutex) {
    disable_interrupt();
    mutex->locked = 0;
    enable_interrupt();
    // uart_send_string("\n\rmutex unlocked");
}