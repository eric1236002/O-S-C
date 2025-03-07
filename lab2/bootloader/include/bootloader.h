#ifndef KERNEL_H
#define KERNEL_H

#include <string.h>
#include "./uart.h" 

#define KERNEL_ADDR 0x80000
#define bootloader 0x10000



char header[4];
int size;
int checksum;

void load_kernel();
void receive_kernel(int size);
void jump_to_kernel();
void kernel();

#endif