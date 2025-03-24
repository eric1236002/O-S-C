#ifndef TIMER_H
#define TIMER_H
#include "uart.h"
#include "utils.h"

#define CORE0_TIMER_IRQ_CTRL ((volatile unsigned int*)0x40000040)

void core_timer_enable();
void core_timer_handler();
void core_timer_disable();
#endif



