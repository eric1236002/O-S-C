#ifndef UTILS_H
#define UTILS_H

#include "uart.h"

#define MAX_INPUT_LEN 256

int hextodec(const char *hex);
int strcmp(const char *str1, const char *str2);
void align(void *ptr, int alignment);
void input_string(char *input_buffer);
unsigned int my_atoi(const char *str);
unsigned int input_int(char *input_buffer);

#endif