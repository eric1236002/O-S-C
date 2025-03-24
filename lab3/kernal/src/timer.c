#include "timer.h"

void core_timer_enable()
{
    unsigned long long freq;
    asm volatile("msr cntp_ctl_el0, %0\n":: "r"(1)); // cntp_ctl_el0 is the control register for the core timer
    *CORE0_TIMER_IRQ_CTRL = 2; //CORE0_TIMER_IRQ_CTRL is the interrupt control register for the core timer 
                               //disable the interrupt, so that the timer interrupt can be used for the kernel
}
void core_timer_handler()
{
    unsigned long long freq, current_time;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("msr cntp_tval_el0, %0":: "r"(freq*2));  
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_time));
    uart_send_string("After boot. Current time: ");
    uart_send_int(current_time/freq);
    uart_send_string("\n");
}

void core_timer_disable()
{
    asm volatile("msr cntp_ctl_el0, %0":: "r"(0));
    *CORE0_TIMER_IRQ_CTRL = 0;
}

