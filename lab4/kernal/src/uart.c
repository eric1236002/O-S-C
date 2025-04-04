#include "uart.h"

// buffer
struct uart_buffer {
    char data[UART_BUFFER_SIZE];
    int read_pos;
    int write_pos;
    int count;
};

// buffer
static struct uart_buffer rx_buffer = {0};
static struct uart_buffer tx_buffer = {0};

// mutex for protecting buffer
static mutex_t rx_mutex = {0};
static mutex_t tx_mutex = {0};

// initialize mutex
void mutex_init(mutex_t *mutex) {
    mutex->locked = 0;
}

// get mutex
void mutex_lock(mutex_t *mutex) {
    disable_interrupt();
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
}

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
    *AUX_MU_LCR = 3;      // set to 8 data bits, no parity bit, 1 stop bit
    *AUX_MU_MCR = 0;      // set RTS line to be always high
    *AUX_MU_BAUD = 270;   // set baud rate to 115200
    *AUX_MU_IIR = 6;      // clear FIFO
    *AUX_MU_CNTL = 3;     // enable receiver and transmitter

    // enable interrupt
    *AUX_MU_IER = 0x3;    // enable interrupt
    *IRQ_ENABLE_1 |= (1 << 29);  // enable UART IRQ
    
    // init buffer
    rx_buffer.read_pos = rx_buffer.write_pos = rx_buffer.count = 0;
    tx_buffer.read_pos = tx_buffer.write_pos = tx_buffer.count = 0;
    
    // initialize mutex
    mutex_init(&rx_mutex);
    mutex_init(&tx_mutex);
}

void uart_send_char(char c) {
    while(!(*AUX_MU_LSR & 0x20)); // wait until can send
    *AUX_MU_IO = c;
}

// get a char from buffer
char uart_async_getc() {
    char c = 0;
    
    // use mutex to protect buffer access
    mutex_lock(&rx_mutex);
    
    if (rx_buffer.count > 0) {
        c = rx_buffer.data[rx_buffer.read_pos];
        rx_buffer.read_pos = (rx_buffer.read_pos + 1) % UART_BUFFER_SIZE;
        rx_buffer.count--;
    }
    
    mutex_unlock(&rx_mutex);
    return c;
}

// put a char to buffer
void uart_async_putc(char c) {
    // use mutex to protect buffer access
    mutex_lock(&tx_mutex);
    
    // check if buffer is full
    if (tx_buffer.count < UART_BUFFER_SIZE) {
        tx_buffer.data[tx_buffer.write_pos] = c;
        tx_buffer.write_pos = (tx_buffer.write_pos + 1) % UART_BUFFER_SIZE;
        tx_buffer.count++;
        
        // enable transmit interrupt
        *AUX_MU_IER |= 0x2;
    }
    
    mutex_unlock(&tx_mutex);
}

// modify interrupt handler
void uart_interrupt_handler() {
    // read interrupt identification register
    int iir = *AUX_MU_IIR & 0x6;
    
    // receive interrupt
    if (iir & 0x4) {
        char c = (char)(*AUX_MU_IO);
        
        // use mutex to protect rx buffer
        mutex_lock(&rx_mutex);
        if (rx_buffer.count < UART_BUFFER_SIZE) {
            rx_buffer.data[rx_buffer.write_pos] = c;
            rx_buffer.write_pos = (rx_buffer.write_pos + 1) % UART_BUFFER_SIZE;
            rx_buffer.count++;
        }
        mutex_unlock(&rx_mutex);
    }
    
    // transmit interrupt
    if (iir & 0x2) {
        // use mutex to protect tx buffer
        mutex_lock(&tx_mutex);
        if (tx_buffer.count > 0) {
            *AUX_MU_IO = tx_buffer.data[tx_buffer.read_pos];
            tx_buffer.read_pos = (tx_buffer.read_pos + 1) % UART_BUFFER_SIZE;
            tx_buffer.count--;
        }
        
        // if transmit buffer is empty, disable transmit interrupt
        if (tx_buffer.count == 0) {
            *AUX_MU_IER &= ~0x2;
        }
        mutex_unlock(&tx_mutex);
    }
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