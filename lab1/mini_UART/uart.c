#include <stdint.h>
#include "uart.h"

static void delay(unsigned int clock) {
    while (clock--) {
        asm volatile("nop");
    }
}

void uart_init(void) {
    // Enable Mini UART
    *AUX_ENABLES |= 1;
    
    // Disable transmitter and receiver during configuration
    *AUX_MU_CNTL = 0;
    
    // Disable interrupts
    *AUX_MU_IER = 0;
    
    // Set data size to 8 bits
    *AUX_MU_LCR = 3;
    
    // Disable auto flow control
    *AUX_MU_MCR = 0;
    
    // Set baud rate to 115200
    *AUX_MU_BAUD = 270;
    
    // Clear FIFO and enable it
    *AUX_MU_IIR = 6;
    
    // Configure GPIO pins 14 and 15
    uint32_t selector = *GPFSEL1;
    selector &= ~((7 << 12) | (7 << 15)); // Clear bits 12-14 (GPIO14) and 15-17 (GPIO15)
    selector |= (2 << 12) | (2 << 15);    // Set alternate function 5 for both pins
    *GPFSEL1 = selector;
    
    // Disable pull-up/down
    *GPPUD = 0;
    delay(150);
    // Set pull-up/down for pins 14 and 15
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    delay(150);
    *GPPUDCLK0 = 0;
    
}

void uart_send_string(const char *str) {
    while (*str) {
        while (!(*AUX_MU_LSR_REG & 0x20)) {} // 等待 UART 可用
        *AUX_MU_IO_REG = (uint32_t)(*str++);
    }
}

char uart_receive_char() {
    while (!(*AUX_MU_LSR_REG & 0x01)) {} // 等待數據準備好
    char c = (char)(*AUX_MU_IO_REG);
    return c == '\r' ? '\n' : c;
}
