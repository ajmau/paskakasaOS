#include <interrupts.h>
#include <text_terminal.h>
#include <stddef.h>

struct interrupt_descriptor idt[256];
extern char vector_0_handler[];

void interrupt_general_handler(cpu_status_t* context)
{
    switch (context->vector_number)
    {
        case 0x13:
            print_string("GENERAL PROTECTION FAULT\n", 25);
            asm ("hlt");
            break;
        case 0x14:
            print_string("PAGE FAULT\n", 11);
            asm ("hlt");          
            break;
        case 0x123:
            //log("yykaakoo\n");
            break;
        default:
            print_string("unexpected interrupt.\n", 22);
            asm  ("hlt");
            break;
    }
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

void setup_interrupts()
{

    int i;
    for (size_t i = 0; i < 256; i++)  {
        set_idt_entry(i, (uint64_t*)(vector_0_handler + (i*16)), 0);
    }

    load_idt(&idt);

}