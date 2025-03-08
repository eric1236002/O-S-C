#include "bootloader.h"

unsigned int checksum = 0;
unsigned int size = 0;
unsigned char header[8];

void load_kernel() {
    uart_send_string("\n\rWaiting for kernel...\n\r");
    
    for(int i = 0; i < 4; i++) {
        header[i] = uart_receive_char();
    }
    size = *(unsigned int*)&header[0];
    
    uart_send_string("\n\rKernel size: ");
    uart_send_int(size);
    uart_send_string(" bytes\n");

    uart_send_string("\n\rReceiving kernel...\r");
    
    receive_kernel(size);
    
    uart_send_string("\n\rKernel loaded, jumping to execution...\r");
    jump_to_kernel();
}

void receive_kernel(int size) {
    char *kernel = (char *)KERNEL_ADDR; 
    
    
    uart_send_string("\n\rstart to receive kernel\r");
    
    for(unsigned int i=0; i < size; i++) {
        kernel[i] = uart_receive_char();;           
    }
}

void jump_to_kernel() {
    typedef void (*kernel_func)(void);
    kernel_func kernel_entry = (kernel_func)KERNEL_ADDR;
    uart_send_string("\n\rJumping to kernel...\n\r");
    
}
