#include <stdint.h>
#include <mem.h>
#include <vga.h>
#include <text_terminal.h>

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
extern uint64_t stack_top;
extern uint64_t stack_bottom;
uint64_t stacktop;
uint64_t stackbottom;

void pmm_print_info()
{
    print("[PMM] region start: ");
    print_hex(pool.base);
    print("[PMM] region length: ");
    print_hex(pool.length);   
    print("[PMM] region end: ");
    print_hex(pool.base+pool.length);
}

void init_pmm(uint32_t e820map)
{
    int foundMemoryArea = 0;
    uint64_t start = (uint64_t)&kernel_start;
    uint64_t end = (uint64_t)&kernel_end;
    stacktop = (uint64_t)&stack_top;
    stackbottom = (uint64_t)&stack_bottom;
    uint64_t klength = end-start;
    // dont hard code this
    uint64_t kphys_start = (uint64_t)0x100000;

    char membuf[40];
    memset(membuf,0,40);
    int i;
    for (i=0; i<50; i++) {
        e820_entry_t* entry  = &((e820_entry_t*)e820map)[i];
        if (entry->base == 0x0 && entry->region_length == 0x0  && entry->region_type == 0x0 && entry->acpi == 0x0) {
            break;
        }
        if (entry->region_type  == 0x1) {
            pmeminfo.total_memory+=entry->region_length;

            if (entry->base >= kphys_start && entry->base <= kphys_start+klength && foundMemoryArea == 0) {
                pool.base = ((kphys_start+klength+0x1000) + 0xFFF) & ~0xFFF; // move past kernel memory and page align
                pool.length = entry->region_length - klength;
                foundMemoryArea++;
                continue;
            }

            // discard low memory
            if (foundMemoryArea == 0 && entry->base != 0x0) {
                pool.base = entry->base;
                pool.length = entry->region_length;
                foundMemoryArea++;
            }

        }
    }
    pool.pages = pool.length / 0x1000;
    for (i = 0; i  < 100; i++) {
        pool.bitmap[i] = 0;
    }


}

uint64_t pmm_allocate()
{
    uint64_t start = (uint64_t)&kernel_start;

    int i;
    for (i=0; i < pool.pages; i++) {
        if (pool.bitmap[i] == 0x0) {
            pool.bitmap[i] = 0x1;

            uint64_t ret = pool.base + (i * PAGESIZE);

            return ret;
        }
    }

    return 0;
}

uint64_t pmm_kallocate()
{
    uint64_t start = (uint64_t)&kernel_start;
    return start+pmm_allocate();
}

uint64_t get_usable_memory()
{
    return pool.length;
}

uint64_t get_total_memory()
{
    return pmeminfo.total_memory;
}