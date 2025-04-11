#include <interrupts.h>
#include <text_terminal.h>
#include <vmm.h>
#include <portio.h>
#include <stddef.h>
#include <apic.h>

struct interrupt_descriptor idt[256];
extern char vector_0_handler[];

void log(char *msg);


void page_fault_handler(cpu_status_t* context)
{
    print("PAGEFAULT\n");
    /*
    print("RAX:  ");
    print_hex(context->rax);
    print("RBX:  ");
    print_hex(context->rbx);
    print("RCX:  ");
    print_hex(context->rcx);
    print("RDX:  ");
    print_hex(context->rdx);
*/
    print("RSP:  ");
    print_hex(context->iret_rsp);
    print("RIP:  ");
    print_hex(context->iret_rip);
    print("ERRROR CODE: ");
    print_hex(context->error_code);
    uint64_t value;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(value));

    print("CR3: ");
    print_hex(value);

//    if (context->error_code & (1 << 0)) {
 //       print("Mapping cr3\n");
        map_page(value, value);
 //   } else  {
   //     print("Halting system\n");
    //    asm("HLT");
  //  }
    asm("hlt");
    return;

}

void panic(cpu_status_t *context)
{
    print("RAX:  ");
    print_hex(context->rax);
    print("RBX:  ");
    print_hex(context->rbx);
    print("RCX:  ");
    print_hex(context->rcx);
    print("RDX:  ");
    print_hex(context->rdx);
    print("RSI:  ");
    print_hex(context->rsi);
    print("RDI:  ");
    print_hex(context->rdi);
    print("RBP:  ");
    print_hex(context->rbp);

    print("IRET RIP:  ");
    print_hex(context->iret_rip);
    print("IRET CS:  ");
    print_hex(context->iret_cs);
     print("IRET FLAGS:  ");
    print_hex(context->iret_flags);
    print("IRET RSP:  ");
    print_hex(context->iret_rsp);
    print("IRET SS:  ");
    print_hex(context->iret_ss);

    print("ERROR CODE: ");
    print_hex(context->error_code);
    print("VECTOR NUMBER:  ");
    print_hex(context->vector_number);
    asm("hlt");
}

int val = 1;

int get_val()
{
    return val;
}

void handle_timer()
{
    print_int(val);
    val++;

    APIC_REG(APIC_EOI) = 0;

}

void interrupt_general_handler(cpu_status_t* context)
{
    switch (context->vector_number)
    {
        case 0x8:
        case 0x20:
        case 0x32:
            handle_timer();
            break;
        case 0x13:
            print("General protection fault!!!\n");
            panic(context);
            break;
        case 0x14:
            page_fault_handler(context);
            break;
        case 0x123:
            //log("yykaakoo\n");
            break;
        default:
            print("Default interrupt handler\n");
            print_hex(context->vector_number);
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
void log(char *msg) {
    const uint16_t COM1 = 0x3f8;

    while (*msg != '\0') {
        outb(COM1, *msg);
        msg++;
    }
}