#ifndef UART_H
#define UART_H

// 基礎地址定義
#define PBASE           0x7E000000
#define AUX_BASE        (PBASE + 0x215000)
#define GPIO_BASE       (PBASE + 0x200000)

// AUX registers
#define AUX_ENABLES     ((volatile uint32_t*)(AUX_BASE + 0x04))
#define AUX_MU_CNTL     ((volatile uint32_t*)(AUX_BASE + 0x60))
#define AUX_MU_IER      ((volatile uint32_t*)(AUX_BASE + 0x44))
#define AUX_MU_LCR      ((volatile uint32_t*)(AUX_BASE + 0x0C))
#define AUX_MU_MCR      ((volatile uint32_t*)(AUX_BASE + 0x50))
#define AUX_MU_BAUD     ((volatile uint32_t*)(AUX_BASE + 0x68))
#define AUX_MU_IIR      ((volatile uint32_t*)(AUX_BASE + 0x48))

// GPIO registers
#define GPFSEL1         ((volatile uint32_t*)(GPIO_BASE + 0x04))
#define GPPUD           ((volatile uint32_t*)(GPIO_BASE + 0x94))
#define GPPUDCLK0       ((volatile uint32_t*)(GPIO_BASE + 0x98))

void uart_init(void);

void uart_send_string(const char *str);

char uart_receive_char();
#endif