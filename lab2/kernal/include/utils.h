#ifndef UTILS_H
#define UTILS_H

#include "uart.h"

#define MAX_INPUT_LEN 100

void input_string(char *input_buffer);
int hextodec(const char *hex);
int strcmp(const char *str1, const char *str2);
void align(void *ptr, int alignment);

#endif