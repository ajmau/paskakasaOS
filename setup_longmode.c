#include <stdint.h>


extern void enable_pae();
extern void switch_to_64(uint64_t, uint32_t);
extern void* _end;

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

// Logs messages into COM1 port.
// Qemu can read these messages with "-serial file:serial.log" option
void log(char *msg) {
    const uint16_t COM1 = 0x3f8;

    while (*msg != '\0') {
        outb(COM1, *msg);
        msg++;
    }
}

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

// Declare an external reference to the "entries" symbol
//extern entry_t entries[NUM_ENTRIES];

__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
//__attribute__((aligned(4096))) uint64_t pmd[512];

// identity map lowest 1gb
void identity_map_1gb()
{
    for (int i = 0; i < 512; i++) {
        pml4[i] = 0;
        pud[i] = 0;
        //pmd[i] = 0;
    }
    pml4[0] = ((uint64_t)pud & 0xFFFFFFFFFF000) | 3;
    pud[0] = (0& 0xFFFFFFFFFF000) |0b10000011;// 3;
 //   pmd[0] = (0 & 0xFFFFFFFFFF000) | 0b10000011;

}

void _setup_lmode(uint32_t memorymap)
{
    identity_map_1gb();
    log("Identity mapped first 2GB\n");
    enable_pae();
    log("Enabled PAE\n");
    log("Switching to long mode\n");
    switch_to_64((uint64_t)&pml4, memorymap);
}
