#ifndef SHELL_H
#define SHELL_H

#include "uart.h"
#include "reboot.h"
#include "loader.h"
#include "utils.h"

char filename_buffer[MAX_INPUT_LEN];
extern char input_buffer[MAX_INPUT_LEN];
extern char *dtb_addr;
extern unsigned long initramfs_start;
#endif