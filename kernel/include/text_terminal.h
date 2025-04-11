#ifndef TEXT_TERMINAL
#define TEXT_TERMINAL
#include <stdint.h>

void init_text_terminal(uint64_t* fonts);
void print(char* msg);
void printxy(char* msg, int, int);
void print_int_xy(int num, int, int);
void print_hex(uint64_t hex);
void print_int(uint64_t num);

#endif