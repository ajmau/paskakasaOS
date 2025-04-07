#ifndef ALLOC_H
#define ALLOC_H

#define BLOCK_FREE 0
#define BLOCK_USED 1

typedef struct block {
    uint64_t size;
    uint8_t status;
    struct block *next;
}  block_t;

typedef struct {
    uint64_t region_start;
    uint64_t region_end;
    uint64_t prev;
} allocator_t;

void install_heap();
void *kalloc(int);

#endif