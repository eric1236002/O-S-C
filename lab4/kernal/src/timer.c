#include "timer.h"

struct timer_event timer_queue[MAX_TIMERS];
int timer_count=0;

void core_timer_enable()
{
    unsigned long long freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("msr cntp_ctl_el0, %0" :: "r"(1));        // enable the timer
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq*2));  // set the timer
    *CORE0_TIMER_IRQ_CTRL = 2;                             // enable the interrupt
}
void core_timer_handler()
{
    uart_send_string("\n\r[DEBUG] Timer interrupt fired!\n\r");
    unsigned long long freq, current_time;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_time));

    process_timers();

    uart_send_string("After boot. Current time: ");
    uart_send_int(current_time/freq);
    uart_send_string("\n");
}

void core_timer_disable()
{
    asm volatile("msr cntp_ctl_el0, %0":: "r"(0));
    *CORE0_TIMER_IRQ_CTRL = 0;
}

// // use to add timer to timer_queue
void add_timer(timer_callback_t callback, void* data, unsigned long long after) {
    if (timer_count >= MAX_TIMERS) {
        uart_send_string("Timer queue is full\n");
        return;
    }
    unsigned long long freq,current_time;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_time));

    if(timer_count <10){
        timer_queue[timer_count].callback = callback;
        timer_queue[timer_count].data = data;
        timer_queue[timer_count].expire_time = current_time + after * freq;
        timer_queue[timer_count].active = 1;
    }
    // if there is only one timer or the new timer is shorter than the first timer

    
    if(timer_count == 0||timer_queue[timer_count].expire_time < timer_queue[0].expire_time){
        
        //count the time to next timer
        unsigned long long time = timer_queue[timer_count].expire_time - current_time;
        uart_send_string("Time to next timer: ");
        uart_send_int(time);
        uart_send_string("\n");
        asm volatile("msr cntp_tval_el0, %0" :: "r"(time));  // set the timer

        asm volatile("msr cntp_ctl_el0, %0" :: "r"(1));        // enable the timer
        *CORE0_TIMER_IRQ_CTRL = 2;                             // enable the interrupt
    }
    timer_count++;
}

// use to process timer_queue
// current time 34         count=3  expire=2
// time queue 12,23,56
void process_timers(){
    unsigned long long freq,current_time;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_time));

    unsigned int still_have=0;
    unsigned long long earliest_time =  ~0ULL;
    for(int i = 0; i < timer_count; i++){
        if(timer_queue[i].expire_time <= current_time){
            timer_queue[i].callback(timer_queue[i].data);
            timer_queue[i].active = 0;
        }
        if(timer_queue[i].active){
            unsigned long long time_to_expire = timer_queue[i].expire_time - current_time;
            if(time_to_expire < earliest_time){
                earliest_time = time_to_expire;
            }
            still_have = 1;
        }
    }

    // for (int i = 0; i < timer_count;)
    // {
    //     if(timer_queue[i].active==0 && timer_count>1){
    //         // bug here
    //         timer_queue[i]=timer_queue[timer_count-1];
    //         timer_count--;
    //     }
    //     else{
    //         i++;
    //     }
    // }
    int i = 0;
    while (i < timer_count) {
        if (timer_queue[i].active == 0) {
            if (i == timer_count - 1) {
                timer_count--;
            } else {
                timer_queue[i] = timer_queue[10];
                timer_count--;
            }
        } else {
            i++;
        }
    }

    if (still_have)
        asm volatile("msr cntp_tval_el0, %0" :: "r"(earliest_time));  // set the timer
    else
        asm volatile("msr cntp_tval_el0, %0" :: "r"(freq*5));  // set the timer

}
void timer_callback(void* data)
{
    char* message=(char*)data;
    unsigned long long current_time,freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_time));
    uart_send_string("\n\rAfter boot. Current time: ");
    uart_send_int(current_time/freq);
    uart_send_string("\n\rMessage: ");
    uart_send_string(message);
    uart_send_string("\n\r");
}
void setTimeout(char* message, unsigned long long seconds){
    char* message_copy = (char*)simple_alloc(string_len(message) + 1);
    if(message_copy == NULL){
        uart_send_string("\n\rError: Failed to allocate memory for message\n\r");
        return;
    }
    

    for (int i = 0; message[i] != '\0'; i++) {
        message_copy[i] = message[i];
    }
    message_copy[string_len(message)] = '\0';
    
    add_timer(timer_callback, message_copy, seconds);
    
    unsigned long long current_time, freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_time));
    
    uart_send_string("\n\rCurrent time: ");
    uart_send_int(current_time/freq);
    uart_send_string("\n\rMessage: ");
    uart_send_string(message);
    uart_send_string("\n\rExpires in ");
    uart_send_int(seconds);
    uart_send_string(" seconds\n\r");
    uart_send_string("Timer count: ");
    uart_send_int(timer_count);
    uart_send_string("\n\r");
}