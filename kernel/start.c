#include <stdint.h>
#include <stddef.h>
#include <interrupts.h>
#include <acpi.h>
#include <apic.h>
#include <mem.h>
#include <pmm.h>
#include <vmm.h>
#include <alloc.h>
#include <portio.h>
#include <vesa.h>
#include <vga.h>
#include <fat32.h>
#include <text_terminal.h>

extern void load_gdt();
void log(char *msg);
void new_page_table(uint32_t addr);
void flush_tlb_all(uint64_t);
void identity_map_1gb();
static void play_sound(uint32_t nFrequence);

uint32_t memorymaps;
uint32_t vesa;
extern uint64_t stack_top;

uint64_t* mem;

uint16_t sounds[5] = { 523, 440, 380, 680, 523 };

void start(uint32_t m, uint32_t v)
{
    // save variables before updating stack
    memorymaps=m;
    vesa=v;

    // setup stack to higher addresses.
    // This is bad way to do it, bootloader should probably handle it.
    uint64_t rbp = (uint64_t)&stack_top;
    uint64_t rsp  = rbp-0x70;
    __asm__ volatile (
        "mov %0, %%rsp\n"
        :
        : "r"(rsp)
        : "rsp"
    ); 
     __asm__ volatile (
        "mov %0, %%rbp\n"
        :
        : "r"(rbp)
        : "rbp"
    ); 
  
    load_gdt();
    init_pmm(memorymaps);
    init_vesa(vesa);
    setup_interrupts();

    // read font file from disk
    mem = (uint64_t*)pmm_allocate();
    mem = (uint64_t*)((uint64_t)mem + 0xFFFFFFFF80000000);

    init_fat();
    read_file("FONT    PSF", (void*)mem);
    init_text_terminal(mem);

    uint32_t fb = get_framebuffer();
    init_paging(fb);

//    pmm_print_info();
    draw_rectangle(150, 100, 100, 100);
    draw_rectangle(300, 400, 50, 100);
    
    //for (int i  =0; i < 10000; i++)  {
    //    print("asdasdasd\n;");
    //}

    parse_acpi();

    apic_t *apic = get_apic();

    print("local apic: ");
    print_hex((uint64_t)apic->local_apic);
    print("IO apic: ");
    print_hex((uint64_t)apic->io_apic);

    for (int x=0; x < 10; x++) {
        printxy("asd", 80, 0);
    }

    init_apic(apic->local_apic, apic->io_apic);

    while (1) {
        for (int y = 0; y < 5; y++) {
            int val = get_val();
            play_sound(sounds[y]);

            while ((get_val() - val) < 100) {}

        }
    }

    while (1) {}
}

static void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
    //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
    //And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
}
