#ifndef UTILS_H
#define UTILS_H

#include "uart.h"

#define MAX_INPUT_LEN 256

typedef struct {
    volatile int locked;
} mutex_t;


void mutex_init(mutex_t *mutex);
void mutex_lock(mutex_t *mutex);
void mutex_unlock(mutex_t *mutex);

int hextodec(const char *hex);
int strcmp(const char *str1, const char *str2);
unsigned long align(unsigned long num, unsigned long alignment);
void input_string(char *input_buffer);
unsigned int my_atoi(const char *str);
unsigned int input_int(char *input_buffer);
char* input_hex(char *input_buffer);
unsigned int string_len(const char *str);
char* itoa(int num, int base);
char* strcpy(char* dest, const char* src);
char *utohx(unsigned int num);

#endif