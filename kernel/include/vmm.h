#ifndef VMM_H
#define VMM_H

void init_paging(uint32_t);
uint64_t alloc(uint32_t);

#endif