#include <stdint.h>
#include "uart.h"

void uart_init(void) {
    register unsigned int r;

    // init GPIO
    r = *GPFSEL1;
    r &= ~((7<<12) | (7<<15)); // clean gpio14, gpio15
    r |= (2<<12) | (2<<15);    // set alt5 for gpio14, gpio15
    *GPFSEL1 = r;

    // disable pull up/down for all GPIO pins & delay for 150 cycles
    *GPPUD = 0;
    for(r=0; r<150; r++) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14) | (1<<15);
    for(r=0; r<150; r++) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;

    // init UART
    *AUX_ENABLES |= 1;    // enable mini UART
    *AUX_MU_CNTL = 0;     // stop the UART
    *AUX_MU_IER = 0;      // stop the UART interrupt
    *AUX_MU_LCR = 3;      // set to 8 data bits, no parity bit, 1 stop bit
    *AUX_MU_MCR = 0;      // set RTS line to be always high
    *AUX_MU_BAUD = 270;   // set baud rate to 115200
    *AUX_MU_IIR = 6;      // clear FIFO
    *AUX_MU_CNTL = 3;     // 啟用收發器
}

void uart_send_char(char c) {
    while(!(*AUX_MU_LSR & 0x20)); // wait until can send
    *AUX_MU_IO = c;
}

char uart_receive_char(void) {
    while(!(*AUX_MU_LSR & 0x01)); // wait until can receive
    char c = (char)(*AUX_MU_IO);
    return c;
}

void uart_send_string(const char* str) {
    while(*str) {
        uart_send_char(*str++);
    }
}

void uart_send_hex(unsigned int hex) {
    char temp[] = "0123456789ABCDEF";
    char output[9];
    output[8] = '\0';
    uart_send_string("0x");
    for(int i = 7; i >= 0; i--) {
        // take the last 4 bits
        output[i] = temp[hex & 0xF];
        hex >>= 4;
    }
    uart_send_string(output);
}

void uart_send_hex64(unsigned long hex) {
    uart_send_string("0x");
    for(int i = 60; i >= 0; i -= 4) {
        unsigned int digit = (hex >> i) & 0xF;
        if(digit < 10)
            uart_send_char('0' + digit);
        else
            uart_send_char('A' + digit - 10);
    }
}

int uart_read_int(void) {
    int num=0;
    char c;
    for(int i=0;i<4;i++){
        c = uart_receive_char();//Uart each time transfer 1 byte(8 bits)
        num=num<<8;//num<<8 is num*256
        num+=(int)c; //c is char, so need to convert to int
    }
    return num;
}

void uart_send_int(int number) {
    char output[12];
    int i = 0;
    if (number == 0) {
        uart_send_char('0');
        return;
    }
    if (number < 0) {
        uart_send_char('-');
        number = -number;
    }
    while (number > 0) {
        output[i++] = number % 10 + '0';
        number /= 10;
    }
    for (int j = i - 1; j >= 0; j--) {
        uart_send_char(output[j]);
    }
}

int uart_is_readable() {
    return (*AUX_MU_LSR & 0x01);
}