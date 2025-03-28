#ifndef TIMER_H
#define TIMER_H
#include "uart.h"
#include "utils.h"
#include "allocator.h"
#define MAX_TIMERS 10
#define MAX_MESSAGE_LENGTH 100
#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)0x40000040)

typedef void (*timer_callback_t)(void* data);

struct timer_event {
    timer_callback_t callback;
    void* data;
    unsigned long long expire_time;
    int active;
    char* message;
};


void add_timer(timer_callback_t callback, void* data, unsigned long long after);
void process_timers();
void setTimeout(char* message, unsigned long long seconds);

void core_timer_enable();
void core_timer_handler();
void core_timer_disable();
#endif



