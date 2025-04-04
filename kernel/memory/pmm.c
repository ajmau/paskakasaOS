#include <stdint.h>
#include <mem.h>

#define PAGESIZE 4096

typedef struct {
    uint64_t base;
    uint64_t region_length;
    uint32_t region_type;
    uint32_t acpi;
} __attribute__((packed)) e820_entry_t;


typedef struct {
    uint64_t base;
    uint64_t length;
    uint64_t pages;
    uint8_t bitmap[100];
} pool_t;

typedef struct {
    uint64_t total_memory;
} pmeminfo_t;

pool_t pool;
pmeminfo_t pmeminfo;

extern uint64_t kernel_end;
extern uint64_t kernel_start;


void init_pmm(uint32_t e820map)
{
    int foundMemoryArea = 0;
    uint64_t start = (uint64_t)&kernel_start;
    uint64_t end = (uint64_t)&kernel_end;

    int i;
    for (i=0; i<50; i++) {
        e820_entry_t* entry  = &((e820_entry_t*)e820map)[i];
        if (entry->base == 0x0 && entry->region_length == 0x0  && entry->region_type == 0x0 && entry->acpi == 0x0) {
            break;
        }
        if (entry->region_type  == 0x1) {
            pmeminfo.total_memory+=entry->region_length;

            if (entry->base >= start && entry->base <= end && foundMemoryArea == 0) {
                pool.base = ((end+0x1000) + 0xFFF) & ~0xFFF; // move past kernel memory and page align
                pool.length = entry->region_length - (end - entry->base); 
                foundMemoryArea++;
                continue;
            }

            // discard low memory
            if (foundMemoryArea == 0 && pool.base != 0x0) {
                pool.base = entry->base;
                pool.length = entry->region_length;
                foundMemoryArea++;
            }
        }
    }

    pool.pages = pool.length / 0x1000;
    for (i = 0; i  < 100; i++)
    pool.bitmap[i] = 0;
}

uint64_t pmm_allocate()
{
    int i;
    for (i=0; i < pool.pages; i++) {
        if (pool.bitmap[i] == 0x0) {
            pool.bitmap[i] = 0x1;

            return pool.base + (i * PAGESIZE);
        }
    }

    return 0;
}

uint64_t get_usable_memory()
{
    return pool.length;
}

uint64_t get_total_memory()
{
    return pmeminfo.total_memory;
}