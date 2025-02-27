#include <stdint.h>

#define PBASE 0x3F000000
#define AUX_BASE (PBASE + 0x00215000)
#define AUX_MU_IO_REG  ((volatile uint32_t*)(AUX_BASE + 0x40))
#define AUX_MU_LSR_REG ((volatile uint32_t*)(AUX_BASE + 0x54))

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
