#include <stdint.h>

extern void enable_pae();
extern void switch_to_64(uint64_t, uint32_t);
void identity_map_1gb();
extern void* _end;

#define NUM_ENTRIES 6

// Structure corresponding to an "entry"
typedef struct {
    uint32_t base_high;    // First 64-bit value (for each entry)
    uint32_t base_low;    // First 64-bit value (for each entry)
    uint32_t region_length_high;    // Second 64-bit value (for each entry)
    uint32_t region_length_low;    // Second 64-bit value (for each entry)
    uint32_t region_type;    // First 32-bit value (for each entry)
    uint32_t acpi;    // Second 32-bit value (for each entry)
} __attribute__((packed)) e820_entry_t;

__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
__attribute__((aligned(4096))) uint64_t pmd[512];
__attribute__((aligned(4096))) uint64_t pmd2[512];

void _setup_lmode(uint32_t memorymap)
{
    identity_map_1gb();
    enable_pae();
    switch_to_64((uint64_t)&pml4, memorymap);
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

void log(char *msg) {
    const uint16_t COM1 = 0x3f8;

    while (*msg != '\0') {
        outb(COM1, *msg);
        msg++;
    }
}

void identity_map_1gb()
{
        for (int i = 0; i < 512; i++) {
        pml4[i] = 0;
        pud[i] = 0;
        pmd[i] = 0;
        pmd2[i] = 0;
    }

    pml4[0] = ((uint64_t)pud & 0xFFFFFFFFFF000) | 3;
    pud[0] = ((uint64_t)pmd & 0xFFFFFFFFFF000) | 3;
    for (int i = 0; i < 20; i++) { 
        pmd[i]  = (i * 0x200000) | 0b10000011;
    }
}