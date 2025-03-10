#ifndef LOADER_H
#define LOADER_H

#include <string.h>
#include "./uart.h" 
#include "./reboot.h"
#define KERNEL_ADDR 0x80000
#define BOOTLOADER_RELOC_ADDR 0x60000 //define the address of bootloader
#define BOOTLOADER_SIZE 0x2000  // 64KB





void load_kernel();
void receive_kernel(int size);
void jump_to_kernel();

#endif