#include <pmm.h>
#include <mem.h>
#include  <vga.h>

#define  KERNEL_ADDR  

#define PRESENT  (1 << 0) 
#define WRITABLE (1 << 1)
#define WRITABLE (1 << 1)
#define PAT      (1 << 7)

#define ADDRESS_MASK 0xFFFFFFFFFF000

#define PT_INDEX(address)   ((address >> 12) & 0x1ff)
#define PMD_INDEX(address)  ((address >> 21) & 0x1ff)
#define PUD_INDEX(address)  ((address >> 30) & 0x1ff)
#define PML4_INDEX(address) ((address >> 39) & 0x1ff)

extern uint64_t kernel_start;
extern uint64_t kernel_end;
uint64_t* pml;
//uint64_t pml[512];

void set_cr3(uint64_t new_cr3) {
    __asm__ volatile(
        "mov %0, %%cr3"  // Load new page table address into CR3
        :
        : "r" (new_cr3)
        : "memory"
    );
}

void map_page(uint64_t vaddress,  uint64_t paddress)
{
        uint64_t pmlidx = PML4_INDEX(vaddress);
        uint64_t pudidx = PUD_INDEX(vaddress);
        uint64_t pmdidx = PMD_INDEX(vaddress);
        uint64_t ptidx = PT_INDEX(vaddress);

        uint64_t *pud = 0;
        uint64_t *pmd = 0;
        uint64_t *pt  = 0;

        if (!(pml[pmlidx] & PRESENT)) {
            pud = (uint64_t*)pmm_allocate();
            memset(pud, 0, 0x1000);
            pml[pmlidx] = ((uint64_t)pud & ADDRESS_MASK) | PRESENT | WRITABLE; 
        }  else {
            pud = (uint64_t*)(pml[pmlidx] & ADDRESS_MASK);
        }

        if (!(pud[pudidx] & PRESENT)) {
            pmd = (uint64_t*)pmm_allocate();
            memset(pmd, 0, 0x1000);
            pud[pudidx] = ((uint64_t)pmd & ADDRESS_MASK) | PRESENT | WRITABLE; 
        }  else {
            pmd = (uint64_t*)(pud[pudidx] & ADDRESS_MASK);
        }

        if (!(pmd[pmdidx] & PRESENT)) {
            pt = (uint64_t*)pmm_allocate();
            memset(pt, 0, 0x1000);
            pmd[pmdidx] = ((uint64_t)pt & ADDRESS_MASK) | PRESENT | WRITABLE; 
        }  else {
            pt = (uint64_t*)(pmd[pmdidx] & ADDRESS_MASK);
        }

        if (!(pt[ptidx] & PRESENT)) {
            pt[ptidx] = (uint64_t)paddress | PAT | PRESENT | WRITABLE;
        }
}

/*
void unmap_page(uint64_t vaddr)
{
        uint64_t pmlidx = PML4_INDEX(vaddress);
        uint64_t pudidx = PUD_INDEX(vaddress);
        uint64_t pmdidx = PMD_INDEX(vaddress);
        uint64_t ptidx = PT_INDEX(vaddress);

        uint64_t *pud = 0;
        uint64_t *pmd = 0;
        uint64_t *pt  = 0;

        if (!(pml[pmlidx] & PRESENT)) {
            pud = (uint64_t*)pmm_allocate();
            memset(pud, 0, 0x1000);
            pml[pmlidx] = ((uint64_t)pud & ADDRESS_MASK) | PRESENT | WRITABLE; 
        }  else {
            pud = (uint64_t*)(pml[pmlidx] & ADDRESS_MASK);
        }

        if (!(pud[pudidx] & PRESENT)) {
            pmd = (uint64_t*)pmm_allocate();
            memset(pmd, 0, 0x1000);
            pud[pudidx] = ((uint64_t)pmd & ADDRESS_MASK) | PRESENT | WRITABLE; 
        }  else {
            pmd = (uint64_t*)(pud[pudidx] & ADDRESS_MASK);
        }

        if (!(pmd[pmdidx] & PRESENT)) {
            pt = (uint64_t*)pmm_allocate();
            memset(pt, 0, 0x1000);
            pmd[pmdidx] = ((uint64_t)pt & ADDRESS_MASK) | PRESENT | WRITABLE; 
        }  else {
            pt = (uint64_t*)(pmd[pmdidx] & ADDRESS_MASK);
        }

        if (!(pt[ptidx] & PRESENT)) {
            pt[ptidx] = 0;
        }
}

*/
void init_paging(uint32_t framebuffer)
{
    
    pml = pmm_allocate();

    memset(pml, 0, 0x1000);
    uint64_t start = (uint64_t)&kernel_start;
    
    int i;
    for (i = 0; i < 3200; i++) {
        map_page(start + (i * 0x1000), i*0x1000);
        //map_page((i * 0x1000), i*0x1000);
        map_page(framebuffer + (i * 0x1000), framebuffer + (i * 0x1000));
    }

    set_cr3((uint64_t)pml);
}

uint64_t alloc(uint32_t n) {
    uint64_t start = (uint64_t)pmm_allocate();
    map_page(start, start);

    int i;
    for (i = 1; i < n; i++) {
        uint64_t more = (uint64_t)pmm_allocate();
        map_page(more, more);
    }

    return start;
}

void map_kernel() {
    uint64_t start = (uint64_t)&kernel_start;
    uint64_t end = (uint64_t)&kernel_end;

    uint64_t high_address = (uint64_t)0xFFFFFFFF80000000;

    end = (end + 0x1000) & ~0xFFF;

    int i;
    for (i=0; i < end; i+=0x1000) {
        map_page(high_address+i, start+i);
    }

}