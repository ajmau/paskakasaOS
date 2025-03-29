#include <stdint.h>


extern void enable_pae();
extern void switch_to_64(uint64_t, uint64_t);
extern void* _end;

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

typedef struct GDTentry {
    uint16_t limit;
    uint16_t base_bottom;
    uint8_t base_middle;
    uint8_t access;
    uint8_t limit_and_flags;
    uint8_t base_top;
} GDTentry;

typedef struct GDTdescriptor {
    uint16_t size;
    uint64_t offset;
} GDTdescriptor;


// Logs messages into COM1 port.
// Qemu can read these messages with "-serial file:serial.log" option
void log(char *msg) {
    const uint16_t COM1 = 0x3f8;

    while (*msg != '\0') {
        outb(COM1, *msg);
        msg++;
    }
}

GDTentry GDT[3] = {
    // null
     {
    .limit = 0x0,
    .base_bottom = 0x0,
    .base_middle = 0x0,
    .access = 0x0,
    .limit_and_flags = 0x0,
    .base_top = 0x0
    },
    // code
    {
    .limit = 0xFFFF,
    .base_bottom = 0x0,
    .base_middle = 0x0,
    .access = 0x9A,
    .limit_and_flags = 0xCF,
    .base_top = 0x0
    },
    // data
    {
    .limit = 0xFFFF,
    .base_bottom = 0x0000,
    .base_middle = 0x0,
    .access = 0x92,
    .limit_and_flags = 0xCF,
    .base_top = 0x0
    },
};

GDTdescriptor GDTdesc;

__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
__attribute__((aligned(4096))) uint64_t pmd[512];

void huge_map_2mb()
{
    for (int i = 0; i < 512; i++) {
        pml4[i] = 0;
        pud[i] = 0;
        pmd[i] = 0;
    }

    pml4[0] = ((uint64_t)pud & 0xFFFFFFFFFF000) | 3;
    pud[0] = ((uint64_t)pmd & 0xFFFFFFFFFF000) | 3;
    pmd[0] = (0 & 0xFFFFFFFFFF000) | 0b10000011;
}

void setupGDT(GDTdescriptor *desc)
{
        desc->size = sizeof(GDT) - 1;
        desc->offset = (uint64_t)GDT;
}

void _setup_lmode()
{
    char *vidmem = (char*)0xb8000;
    char *msg = "HELLO from protected mode";
    uint16_t length = 26;

    huge_map_2mb();
    setupGDT(&GDTdesc);

    enable_pae();
    int i = 0;
    while (i < length) {
        *vidmem = msg[i];
        vidmem++;
        *vidmem = 0x4;
        vidmem++;
        i++;
    }

    switch_to_64((uint64_t)&pml4, (uint64_t)&GDTdesc);

    log("Log message 1\n");
    log("log message 2\n");

    
}
