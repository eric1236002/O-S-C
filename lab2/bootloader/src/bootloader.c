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
    int timeout_count = 0;
    const int TIMEOUT_MAX = 5000000;//大約3秒
    
    uart_send_string("\n\rstart to receive kernel\n");
    
    for(unsigned int i=0; i < size; i++) {
        timeout_count = 0;
        while(!uart_is_readable() && timeout_count < TIMEOUT_MAX) {
            timeout_count++;
        }
        if(timeout_count >= TIMEOUT_MAX) {
            uart_send_string("\n\rReceive timeout!\r");
            reset(100);
            return;
        }
        kernel[i] = uart_receive_char();
        if((i % 1024) == 0) {
            uart_send_string("."); 
        }
    }
    uart_send_string("\n\rKernel receive complete!\r");
}

void jump_to_kernel() {
    typedef void (*kernel_func)(void);
    kernel_func kernel_entry = (kernel_func)KERNEL_ADDR;
    uart_send_string("\n\rJumping to kernel...\n\r");
    kernel_entry();
}
