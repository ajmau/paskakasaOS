#ifndef MEM_H
#define MEM_H
#include <stddef.h>
#include <stdint.h>

char * itoa( uint32_t value, char * str, int base);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void *memset(void *str, int c, size_t n);
uint8_t strcmp(char *str1, char *str2, size_t len);
uint8_t strlen(char*);
#endif