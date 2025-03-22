#ifndef EXCEPTION_H
#define EXCEPTION_H
#include "uart.h"

#define SynchronousEL1t 1
#define IRQEL1t 2
#define FIQEL1t 3
#define ErrorEL1t 4

#define SynchronousEL1h 5
#define IRQEL1h 6
#define FIQEL1h 7
#define ErrorEL1h 8

#define SynchronousEL0_32 9
#define IRQEL0_32 10
#define FIQEL0_32 11
#define ErrorEL0_32 12

#define SynchronousEL0_64 13
#define IRQEL0_64 14
#define FIQEL0_64 15
#define ErrorEL0_64 16

void exception_entry(unsigned int type);

#endif

