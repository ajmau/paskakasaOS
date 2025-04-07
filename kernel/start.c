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

void log(char *msg);
void new_page_table(uint32_t addr);
void flush_tlb_all(uint64_t);
void identity_map_1gb();
__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
__attribute__((aligned(4096))) uint64_t pmd[512];
__attribute__((aligned(4096))) uint64_t pmd2[512];
__attribute__((aligned(16))) uint8_t  stack[4096];


void start(uint32_t memorymaps, uint32_t vesa)
{
    uint64_t rsp;
    uint64_t rbp;

    __asm__ volatile (
        "mov %%rsp, %0\n"  
        "mov %%rbp, %1\n"  
        : "=r" (rsp), "=r" (rbp)  
        :                       
        : "memory"               
    );

    rsp = (uint64_t)0xFFFFFFFF80000000+rsp;
    rbp = (uint64_t)0xFFFFFFFF80000000+rbp;

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


   //init_vga();
    init_pmm(memorymaps);
    init_vesa(vesa);

     
    uint32_t fb = get_framebuffer();

    setup_interrupts();

    init_paging(fb);
    // read font file from disk
    uint64_t *mem = (uint64_t*)pmm_kallocate();
    init_fat();
    read_file("FONT    PSF", (void*)mem);
    init_text_terminal(mem);


    draw_rectangle(150, 100, 100, 100);
    draw_rectangle(300, 400, 50, 100);

    char totalmsg[] = "Total memory: ";
    char usablemsg[] = "Usable memory: ";
    uint32_t total = get_total_memory();
    uint32_t usable = get_usable_memory();

    usable = usable / (1024*1024);
    total = total / (1024*1024);

    install_heap();

    uint64_t *a = (uint64_t*)kalloc(sizeof(uint64_t)*1);
    *a = 10;
    //*(a+1) = 20;
    uint64_t *b = (uint64_t*)kalloc(sizeof(uint64_t)*2);
    *b  = 123;
    uint64_t *c = (uint64_t*)kalloc(sizeof(uint64_t)*3);
    *c = 99999;

    asm("nop");

    while (1) {}
}
