#include "timer.h"

void core_timer_enable()
{
    unsigned long long freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("msr cntp_ctl_el0, %0" :: "r"(1));        // 啟用計時器
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq*2));  // 設置2秒超時
    *CORE0_TIMER_IRQ_CTRL = 2;                             // 啟用中斷
}
void core_timer_handler()
{
    unsigned long long freq, current_time;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq*2));  // 設置下一個2秒超時
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

