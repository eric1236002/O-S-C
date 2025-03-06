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
    *AUX_MU_CNTL = 0;     // 停用收發器，確保設定過程中 UART 不會意外發送錯誤數據。
    *AUX_MU_IER = 0;      // 停用中斷，通常使用 同步輪詢（Polling） 而不是中斷。
    *AUX_MU_LCR = 3;      // 設定為 8-bit 資料模式（標準 UART 通訊格式）。
    *AUX_MU_MCR = 0;      // 停用流量控制，主要控制 RTS/CTS 硬體流量控制
    *AUX_MU_BAUD = 270;   // 設定 115200 ，Raspberry Pi System Clock 預設是 250 MHz（250,000,000 Hz)，確保 UART 以標準 115200 bps 速率通訊。
    *AUX_MU_IIR = 6;      // 啟用 FIFO，確保 UART FIFO 是乾淨的，避免初始化時發生錯誤
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
        // 取出最後 4 個位元組
        output[i] = temp[hex & 0xF];
        hex >>= 4;
    }
    uart_send_string(output);
}
