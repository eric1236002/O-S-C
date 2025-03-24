#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "uart.h"
#include "timer.h"

#define SynchronousEL1t 0
#define IRQEL1t 1
#define FIQEL1t 2
#define ErrorEL1t 3

#define SynchronousEL1h 4
#define IRQEL1h 5
#define FIQEL1h 6
#define ErrorEL1h 7

#define SynchronousEL0_32 8
#define IRQEL0_32 9
#define FIQEL0_32 10
#define ErrorEL0_32 11

#define SynchronousEL0_64 12
#define IRQEL0_64 13
#define FIQEL0_64 14
#define ErrorEL0_64 15

void exception_entry(unsigned int type);

#endif

