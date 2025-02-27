#include <stdint.h>
#include "uart.h"

void uart_init(void) {
    register unsigned int r;

    // 初始化 GPIO
    r = *GPFSEL1;
    r &= ~((7<<12) | (7<<15)); // 清除 GPIO 14, 15 的設定
    r |= (2<<12) | (2<<15);    // 設定為 ALT5 模式
    *GPFSEL1 = r;

    // 停用上拉/下拉電阻
    *GPPUD = 0;
    for(r=0; r<150; r++) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14) | (1<<15);
    for(r=0; r<150; r++) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;

    // 初始化 UART
    *AUX_ENABLES |= 1;    // 啟用 mini UART
    *AUX_MU_CNTL = 0;     // 停用收發器
    *AUX_MU_IER = 0;      // 停用中斷
    *AUX_MU_LCR = 3;      // 8 位元資料模式
    *AUX_MU_MCR = 0;      // 停用流量控制
    *AUX_MU_BAUD = 270;   // 設定 115200 鮑率
    *AUX_MU_IIR = 6;      // 清除並啟用 FIFO
    *AUX_MU_CNTL = 3;     // 啟用收發器
}

void uart_send_char(char c) {
    while(!(*AUX_MU_LSR & 0x20)); // 等待可以傳送
    *AUX_MU_IO = c;
}

char uart_receive_char(void) {
    while(!(*AUX_MU_LSR & 0x01)); // 等待有資料可讀
    return *AUX_MU_IO;
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
        output[i] = temp[hex & 0xF];
        hex >>= 4;
    }
    uart_send_string(output);
}
