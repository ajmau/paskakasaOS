#include <stdint.h>
#include <stddef.h>

void load_idt(void*);
void set_idt_entry(uint8_t vector, void* handler, uint8_t dpl);
void log(char *msg);
void write_message(char *message, uint32_t len, uint32_t line);

char * itoa( int value, char * str, int base );
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

typedef struct {
    uint64_t base;    // First 64-bit value (for each entry)
    uint64_t region_length;    // Second 64-bit value (for each entry)
    uint32_t region_type;    // First 32-bit value (for each entry)
    uint32_t acpi;    // Second 32-bit value (for each entry)
} __attribute__((packed)) e820_entry_t;

void start(uint32_t memorymap)
{
    char *vidmem = (char*)0xb8000;

    *vidmem = 'X';
    
    e820_entry_t *entries[5];
    e820_entry_t *memmap = (e820_entry_t*)memorymap;

    uint32_t usableMem = 0;
    int i=0;
    for (i=0; i<5; i++) {
        e820_entry_t* entry  = &memmap[i];
        if (entry->base == 0x0 && entry->region_length == 0x0  && entry->region_type == 0x0 && entry->acpi == 0x0) {
            break;
        }
        if (entry->region_type  == 0x1) {
            usableMem+=entry->region_length;
        }
    }

    __asm__ volatile ("xchg %%bx, %%bx" ::: "bx");
    for (size_t i = 0; i < 256; i++)  {
        set_idt_entry(i, (uint64_t*)(vector_0_handler + (i*16)), 0);
    }

    usableMem = usableMem / (1024*1024);
    char *sizeBuffer[100];
    itoa(usableMem, sizeBuffer, 10);


    write_message("Memory in megabytes:", 19, 5);
    write_message(sizeBuffer, 5, 6);

    __asm__ volatile ("xchg %%bx, %%bx" ::: "bx");



    load_idt(&idt);
    __asm__ volatile ("sti");

    write_message("Moi mita kuuluu", 15, 1);
    //__asm__ volatile ("int $123");

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

void write_message(char *message, uint32_t len, uint32_t line) 
{
    char *buffer = (char*)(0xb8000+(line*160));

    uint32_t i;
    for (i=0; i < len; i++) {
        *buffer++ = message[i];
        *buffer++ = 0x0a;
    }
}

char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}