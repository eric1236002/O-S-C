#include "exception.h"

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
            // uart_send_string("\n\rSynchronousEL1h");
            break;
        case IRQEL1h:
            // uart_send_string("\n\rIRQEL1h");
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
            break;
        case IRQEL0_64:
            core_timer_enable();
            core_timer_handler();
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

