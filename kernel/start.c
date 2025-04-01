#include <stdint.h>
#include <stddef.h>

void load_idt(void*);
void set_idt_entry(uint8_t vector, void* handler, uint8_t dpl);
void log(char *msg);
struct cpu_status_t
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t vector_number;
    uint64_t error_code;

    uint64_t iret_rip;
    uint64_t iret_cs;
    uint64_t iret_flags;
    uint64_t iret_rsp;
    uint64_t iret_ss;
};
struct interrupt_descriptor
{
    uint16_t address_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t address_mid;
    uint32_t address_high;
    uint32_t reserved;
} __attribute__((packed));

struct idtr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));


struct interrupt_descriptor idt[256];

extern char vector_0_handler[];

void start()
{
    char *vidmem = (char*)0xb8000;

    *vidmem = 'X';
    
    __asm__ volatile ("xchg %%bx, %%bx" ::: "bx");
    for (size_t i = 0; i < 256; i++)  {
        set_idt_entry(i, (uint64_t*)(vector_0_handler + (i*16)), 0);
    }

    __asm__ volatile ("xchg %%bx, %%bx" ::: "bx");



    load_idt(&idt);
    __asm__ volatile ("sti");

    __asm__ volatile ("int $123");
    while (1) {}
}

void interrupt_general_handler(struct cpu_status_t* context)
{
    switch (context->vector_number)
    {
        case 13:
            log("general protection fault.");
            break;
        case 14:
            log("page fault.");
            break;
        case 0x123:
            log("yykaakoo\n");
            break;
        default:
            log("unexpected interrupt.\n");
            break;
    }
    char *vidmem = (char*)0xb8000;
    *vidmem++ = 'G';
    *vidmem++ = 0xA;
    *vidmem++ = 'A';
    *vidmem++ = 0xb;
    *vidmem++ = 'Y';
    *vidmem++ = 0xD;
    asm("nop");
}

void set_idt_entry(uint8_t vector, void* handler, uint8_t dpl)
{
    uint64_t handler_addr = (uint64_t)handler;

    struct  interrupt_descriptor* entry = &idt[vector];
    entry->address_low = (handler_addr >> 0x00) & 0xFFFF;
    entry->address_mid = (handler_addr >> 0x10) & 0xFFFF;
    entry->address_high =(handler_addr >> 0x20) & 0xFFFFFFFF;
    //your code selector may be different!
    entry->selector = 0x8;
    //trap gate + present + DPL
    entry->flags = 0b1110 | ((dpl & 0b11) << 5) |(1 << 7);
    //ist disabled
    entry->ist = 0;
}

void load_idt(void* idt_addr)
{
    struct idtr idt_reg;
    idt_reg.limit = 0xFFF;
    idt_reg.base = (uint64_t)idt_addr;
    asm volatile("lidt %0" :: "m"(idt_reg));
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
