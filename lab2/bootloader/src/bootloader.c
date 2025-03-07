#include "bootloader.h"



void load_kernel() {
    uart_send_string("Waiting for kernel...\n");

    size = uart_read_int();  // 讀取 size
    checksum = uart_read_int();  // 讀取 checksum


    uart_send_string("Receiving kernel...\n");
    receive_kernel(size);

    uart_send_string("Kernel loaded, jumping to execution...\n");
    jump_to_kernel();
}

void receive_kernel(int size) {
    char *kernel = (char *)KERNEL_ADDR; 
    int cchecksum = 0;
    for (int i = 0; i < size; i++) {
        char c = uart_receive_char();
        cchecksum += c;
        kernel[i] = c;
    }

    if (cchecksum != checksum) {
        uart_send_string("Checksum mismatch!\n");
        return;
    }

    uart_send_string("Checksum OK!\n");
}

void jump_to_kernel() {
    typedef void (*kernel_func)(void);
    kernel_func kernel_entry = (kernel_func)KERNEL_ADDR;
    uart_send_string("Jumping to kernel...\n");
    kernel_entry();
}
