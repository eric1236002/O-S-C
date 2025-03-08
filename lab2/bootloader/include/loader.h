#ifndef LOADER_H
#define LOADER_H

#include <string.h>
#include "./uart.h" 
#include "./reboot.h"
#define KERNEL_ADDR 0x80000
#define bootloader 0x10000





void load_kernel();
void receive_kernel(int size);
void jump_to_kernel();

#endif