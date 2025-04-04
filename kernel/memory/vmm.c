#include <pmm.h>
#include <mem.h>

#define PRESENT 0x1

#define PT_INDEX(address)   ((address >> 12) & 0x1ff)
#define PMD_INDEX(address)  ((address >> 21) & 0x1ff)
#define PUD_INDEX(address)  ((address >> 30) & 0x1ff)
#define PML4_INDEX(address) ((address >> 39) & 0x1ff)

uint64_t* pml;

void set_cr3(uint64_t new_cr3) {
    __asm__ volatile(
        "mov %0, %%cr3"  // Load new page table address into CR3
        :
        : "r" (new_cr3)
        : "memory"
    );
}

void map_page(uint64_t address)
{
        uint64_t pmlidx = PML4_INDEX(address);
        uint64_t pudidx = PUD_INDEX(address);
        uint64_t pmdidx = PMD_INDEX(address);
        uint64_t ptidx = PT_INDEX(address);

        uint64_t *pud = 0;
        uint64_t *pmd = 0;
        uint64_t *pt  = 0;

        if (!(pml[pmlidx] & PRESENT)) {
            pud = (uint64_t*)pmm_allocate();
            memset(pud, 0, 0x1000);
            pml[pmlidx] = ((uint64_t)pud & 0xFFFFFFFFFF000) | 3; 
        }  else {
            pud = (uint64_t*)(pml[pmlidx] & 0xFFFFFFFFFF000);
        }

        if (!(pud[pudidx] & PRESENT)) {
            pmd = (uint64_t*)pmm_allocate();
            memset(pmd, 0, 0x1000);
            pud[pudidx] = ((uint64_t)pmd & 0xFFFFFFFFFF000) | 3; 
        }  else {
            pmd = (uint64_t*)(pud[pudidx] & 0xFFFFFFFFFF000);
        }

        if (!(pmd[pmdidx] & PRESENT)) {
            pt = (uint64_t*)pmm_allocate();
            memset(pt, 0, 0x1000);
            pmd[pmdidx] = ((uint64_t)pt & 0xFFFFFFFFFF000) | 3; 
        }  else {
            pt = (uint64_t*)(pmd[pmdidx] & 0xFFFFFFFFFF000);
        }

        if (!(pt[ptidx] & PRESENT)) {
            pt[ptidx] = (uint64_t)address | 0b10000011;
        }


}

void init_paging(uint32_t framebuffer)
{
    pml = pmm_allocate();

    int i;
    // identity map low memory  and framebuffer
    for (i = 0; i < 3200; i++) {
        map_page(i * 0x1000);
        map_page(framebuffer + (i * 0x1000));
    }

    set_cr3((uint64_t)pml);
}

uint64_t alloc(uint32_t n) {
    uint64_t start = (uint64_t)pmm_allocate();
    map_page(start);

    int i;
    for (i = 0; i < n; i++) {
        uint64_t more = (uint64_t)pmm_allocate();
        map_page(more);
    }

    return start;
}