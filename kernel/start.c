#include <stdint.h>
#include <stddef.h>
#include <interrupts.h>
#include <mem.h>
#include <pmm.h>
#include <vmm.h>
#include <alloc.h>
#include <vesa.h>
#include <vga.h>
#include <fat32.h>
#include <text_terminal.h>

extern void load_gdt();
void gdt_init();
void log(char *msg);
void new_page_table(uint32_t addr);
void flush_tlb_all(uint64_t);
void identity_map_1gb();
__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
__attribute__((aligned(4096))) uint64_t pmd[512];
__attribute__((aligned(4096))) uint64_t pmd2[512];
__attribute__((aligned(16))) uint8_t  stack[4096];

typedef struct {
    uint16_t limit_low;        // Lower 16 bits of the segment limit
    uint16_t base_low;         // Lower 16 bits of the base address
    uint8_t  base_middle;      // Next 8 bits of the base address
    uint8_t  access;           // Access flags (read/write, executable, etc.)
    uint8_t  limit_high;       // Upper 4 bits of the segment limit
    uint8_t  base_high;        // Upper 8 bits of the base address
} __attribute__((packed)) GDTEntry;

typedef struct {
    uint16_t limit;            // GDT limit
    uint64_t base;             // GDT base address
} __attribute__((packed)) GDTR;

GDTEntry gdt[3];
GDTR gdtr;


struct __attribute__((packed)) gdtr_t {
    uint16_t limit;
    uint64_t base;
};

uint32_t memorymaps;
uint32_t vesa;
extern uint64_t stack_top;

uint64_t* mem;

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

    draw_rectangle(150, 100, 100, 100);
    draw_rectangle(300, 400, 50, 100);
    print("moi\n");
    
    for (int i = 0; i < 10; i++) {
        print("asdasd\n");
    }

    char totalmsg[] = "Total memory: ";
    char usablemsg[] = "Usable memory: ";
    uint32_t total = get_total_memory();
    uint32_t usable = get_usable_memory();

    usable = usable / (1024*1024);
    total = total / (1024*1024);

/*
    uint64_t *a = (uint64_t*)kalloc(sizeof(uint64_t)*1);
    *a = 10;
    //*(a+1) = 20;
    uint64_t *b = (uint64_t*)kalloc(sizeof(uint64_t)*2);
    *b  = 123;
    uint64_t *c = (uint64_t*)kalloc(sizeof(uint64_t)*3);
    *c = 99999;

    asm("nop");
*/
    // page fault
    uint64_t* asd = (uint64_t*)0x0;
    *asd=1;

    while (1) {}
}

#define GRAN_4K   (1 << 7)
#define SZ_32     (1 << 6)
#define LONG_MODE (1 << 5)
#define PRESENT        (1 << 7)
#define NOT_SYS        (1 << 4)
#define EXEC           (1 << 3)
#define DC             (1 << 2)
#define RW             (1 << 1)
#define ACCESSED       (1 << 0)


void gdt_init() {
    // Null segment descriptor (invalid)
    gdt[1] = (GDTEntry){0};

    // Kernel code segment 
    gdt[1] = (GDTEntry){
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_middle = 0x00,
        .access = PRESENT | NOT_SYS | EXEC | RW,  // Present, read/write, executable, privilege level 0
        .limit_high = GRAN_4K | LONG_MODE | 0xF,
        .base_high = 0x00,
    };

    // Kernel data segment (64-bit, read/write, privilege level 0)
    gdt[2] = (GDTEntry){
        .limit_low = 0xFFFF,
        .base_low = 0x0000,
        .base_middle = 0x00,
        .access = PRESENT |  NOT_SYS | RW,  // Present, read/write, executable, privilege level 0
        .limit_high = GRAN_4K | LONG_MODE | 0xF,
        .base_high = 0x00,
    };

    // Set up the GDTR (Global Descriptor Table Register)
    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    asm("cli");
    // Load the new GDT
//    asm("lgdt %0" : : "m"(gdtr));

    // Reload the segment registers
 /*   
    __asm__ volatile (
        "mov $0x10, %ax\n"   // Kernel code segment index
        "mov %ax, %ds\n"     // Data segment
        "mov %ax, %es\n"     // Extra segment
        "mov %ax, %fs\n"     // FS segment
        "mov %ax, %gs\n"     // GS segment
        "mov %ax, %ss\n"     // Stack segment
        "mov $0x08, %ax\n"   // Kernel code segment index
        "mov %ax, %cs\n"     // Code segment
    );
   */ 
   load_gdt(&gdtr);
    asm("sti");
    asm("xchg %bx, %bx");
}
