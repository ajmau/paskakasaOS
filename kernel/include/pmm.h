#ifndef PMM_H
#define PMM_H
#include  <stdint.h>

void init_pmm(uint32_t);
uint64_t* pmm_allocate();
void pmm_print_info();
uint64_t* pmm_kallocate();
uint64_t get_usable_memory();
uint64_t get_total_memory();


#endif