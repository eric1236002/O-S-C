#ifndef SHELL_H
#define SHELL_H

#include "uart.h"
#include "reboot.h"
#include "utils.h"
#include "cpio.h"
#include "allocator.h"
#include "fdt.h"
#include "buddy.h"
extern void *dtb_addr;

extern void initramfs_callback(const char *node_name, const char *property_name, const void *property_value);
#endif