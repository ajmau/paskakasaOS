#include <stdint.h>
#include <alloc.h>
#include <vmm.h>


extern uint64_t kernel_end;
allocator_t allocator;

block_t block;

// keeps track of latest known free heap block
block_t *free_ptr;

uint64_t *heap_start;
uint64_t heap_size;

/* 
    TODO:
    - freeing blocks
    - checks for if heap is full
*/

void install_heap()
{
    // initialize heap pointers
    heap_start = (uint64_t*)((((uint64_t)&kernel_end)+0x1000)  & ~0xFFF);
    heap_size = 128*1024;
    free_ptr = heap_start; 

    // setup first free block, which takes the whole heap size
    block_t *first = heap_start;
    first->size = heap_size;
    first->next = 0x0;
    first->status = BLOCK_FREE;

}

block_t *split_block(block_t *ptr, int n)
{
    block_t *newfreeblock = (block_t*)((uint64_t*)ptr+n);

    newfreeblock->size = ptr->size - n;
    newfreeblock->next = ptr->next;
    newfreeblock->status = BLOCK_FREE;

    ptr->size = n;
    ptr->status = BLOCK_USED;
    ptr->next = newfreeblock;

    free_ptr = newfreeblock;
    
    return ptr;
}

void *kalloc(int n)
{
    // check if free block pointers contains large enough block
    if (free_ptr->status == BLOCK_FREE && free_ptr->size => n) {
        return (void*)(split_block(free_ptr, n)+1);
    }

    // if free pointer points to too small, block search linked list for large enough block
    block_t *ptr = heap_start;
    while (ptr != 0x0) {
        if (ptr->status  == BLOCK_FREE && ptr->size => n) {
            return (void*)(split_block(ptr, n)+1);
        }
        ptr = ptr->next;
    }

    return 0x0;

}