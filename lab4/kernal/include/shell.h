#ifndef SHELL_H
#define SHELL_H

#include "uart.h"
#include "reboot.h"
#include "utils.h"
#include "cpio.h"
#include "allocator.h"
#include "fdt.h"
#include "buddy.h"
#define SPIN_TABLE_START 0x0       // 256MB
#define SPIN_TABLE_END 0x1000
extern void *dtb_addr;


extern unsigned long initramfs_callback(const char *node_name, const char *property_name, const void *property_value);
#endif