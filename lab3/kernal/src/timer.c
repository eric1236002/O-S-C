#include "timer.h"

void core_timer_enable()
{
    unsigned long long freq;
    asm volatile("msr cntp_ctl_el0, %0\n":: "r"(1)); // cntp_ctl_el0 is the control register for the core timer ->bit 0 is enable
    asm volatile("mrs %0, cntfrq_el0\n" : "=r"(freq)); // cntfrq_el0 is the frequency of the timer.(how any ticks per second)
    asm volatile("msr cntp_tval_el0, %0\n":: "r"(freq*2)); // cntp_tval_el0 is used to set the timer value.

    *CORE0_TIMER_IRQ_CTRL = 2; //CORE0_TIMER_IRQ_CTRL is the interrupt control register for the core timer 
                               //disable the interrupt, so that the timer interrupt can be used for the kernel
}
void core_timer_handler()
{
    unsigned int freq,current_time;
    asm volatile("mrs x0, cntfrq_el0\n" : "=r"(freq));
    asm volatile("msr cntp_tval_el0, %0\n":: "r"(freq*2));  
    asm volatile("mrs x0, cntpct_el0\n" : "=r"(current_time));
    uart_send_string("After boot. Current time: ");
    uart_send_int(divide(current_time/freq));
    uart_send_string("\n");
}
