#include "exception.h"

void enable_interrupt() { 
    asm volatile("msr DAIFClr, 0xf"); 
}

void disable_interrupt() { 
    asm volatile("msr DAIFSet, 0xf"); 
}

void uart_interrupt_enable() {
    /*
    AUX_MU_IER:    If this bit is set the interrupt line is asserted whenever
                   the receive FIFO holds at least 1 byte.
    */
    *AUX_MU_IER |= 0x3;
}

void uart_interrupt_disable() {
    *AUX_MU_IER &= ~(0x3);
}

void irq_handler()
{
    disable_interrupt();
    //check core timer interrupt
    if (*CORE0_TIMER_IRQ_CTRL & 0x2) {
        core_timer_handler();
    }
    
    //check uart interrupt
    unsigned int uart = (*IRQ_ENABLE_1 & (1<<29));
    if (uart) {
        uart_interrupt_handler();
    }
    
    enable_interrupt();
}


void exception_entry(unsigned int type) {
    uart_send_string("\n\rException type: ");
    uart_send_hex(type);
    uart_send_string("\n\r");
    switch (type) {
        case SynchronousEL1t:
            // uart_send_string("\n\rSynchronousEL1t");
            break;
        case IRQEL1t:
            // uart_send_string("\n\rIRQEL1t");
            break;
        case FIQEL1t:
            // uart_send_string("\n\rFIQEL1t");
            break;
        case ErrorEL1t:
            // uart_send_string("\n\rErrorEL1t");
            break;
        case SynchronousEL1h:
            unsigned long e;
            uart_send_string("\n\rSynchronous exception at EL1h\n\r");
            asm volatile("mrs %0, esr_el1" : "=r"(e));
            uart_send_string("\n\rESR_EL1: ");
            uart_send_hex(e);
            uart_send_string("\n\r");
            break;
        case IRQEL1h:
            disable_interrupt();
            irq_handler();
            enable_interrupt();
            break;
        case FIQEL1h:
            // uart_send_string("\n\rFIQEL1h");
            break;
        case ErrorEL1h:
            // uart_send_string("\n\rErrorEL1h");
            break;  
        case SynchronousEL0_32:
            // uart_send_string("\n\rSynchronousEL0_32");
            break;
        case IRQEL0_32:
            // uart_send_string("\n\rIRQEL0_32");
            break;
        case FIQEL0_32:
            // uart_send_string("\n\rFIQEL0_32");
            break;
        case ErrorEL0_32:
            // uart_send_string("\n\rErrorEL0_32");
            break;
        case SynchronousEL0_64:
            disable_interrupt();
            unsigned long spsr, elr, esr;
            asm volatile("mrs %0, spsr_el1" : "=r"(spsr));
            asm volatile("mrs %0, elr_el1" : "=r"(elr));
            asm volatile("mrs %0, esr_el1" : "=r"(esr));
            
            
            uart_send_string("\n\rException Info:\n\r");
            uart_send_string("SPSR_EL1: ");
            uart_send_hex(spsr);
            uart_send_string("\n\rELR_EL1: ");
            uart_send_hex(elr);
            uart_send_string("\n\rESR_EL1: ");
            uart_send_hex(esr);
            uart_send_string("\n\r");
            enable_interrupt();
            break;
        case IRQEL0_64:
            disable_interrupt();
            irq_handler();
            enable_interrupt();
            break;
        case FIQEL0_64:
            // uart_send_string("\n\rFIQEL0_64");
            break;
        case ErrorEL0_64:
            // uart_send_string("\n\rErrorEL0_64");
            break;
        default:
            uart_send_string("\n\rUnhandled exception type: ");
            uart_send_hex(type);
            uart_send_string("\n\r");
            break;
    }
}

