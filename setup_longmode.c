#include <stdint.h>


extern void enable_pae();
extern void switch_to_64(uint64_t);
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

void _setup_lmode()
{
    char *vidmem = (char*)0xb8000;
    char *msg = "HELLO from protected mode";
    uint16_t length = 26;

    identity_map_1gb();
    enable_pae();
    switch_to_64((uint64_t)&pml4);

    log("Log message 1\n");
    log("log message 2\n");

    
}
