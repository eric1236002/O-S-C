#ifndef LOADER_H
#define LOADER_H

#include <string.h>
#include "./uart.h" 
#include "./reboot.h"
#define KERNEL_ADDR 0x80000
#define BOOTLOADER_RELOC_ADDR 0x60000
// 定義bootloader的大小，用於重定位
#define BOOTLOADER_SIZE 0x10000  // 64KB，應該足夠容納bootloader





void load_kernel();
void receive_kernel(int size);
void jump_to_kernel();

#endif