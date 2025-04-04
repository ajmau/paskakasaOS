#include <stdint.h>
#include <stddef.h>
#include <interrupts.h>
#include <vesa.h>
#include <fat32.h>
#include <font.h>

void log(char *msg);
char * itoa( int value, char * str, int base );
void new_page_table(uint32_t addr);
void flush_tlb_all(uint64_t);
void identity_map_1gb();
__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
__attribute__((aligned(4096))) uint64_t pmd[512];
__attribute__((aligned(4096))) uint64_t pmd2[512];


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
    int i;

    // read font file from disk
    init_fat();
    read_file("FONT    PSF", (void*)0x2000000);

    init_vesa(memorymap, (uint64_t*)0x2000000);

    // identity map framebuffer
    uint32_t fb = get_framebuffer();
    new_page_table(fb);
    flush_tlb_all((uint64_t)&pml4);

    print_string("öngh\n", 6);

    put_pixel(100, 100);

    int test = 0;
    for (i=0; i < 80; i++,test++) {
        if (test == 3) {
            print_string("öngh\n", 6);
            test = 0;
        } else {
        print_string("Fontti testi\n", 13);
        }
    }
    
    print_string("ABC", 3);
    print_string("DEF", 3);
    print_string("GHI", 3);
    print_string("JKL", 3);
    print_string("MNO", 3);
    print_string("PQR", 3);
    print_string("STU", 3);

    print_string("uusi rivi\n", 10);
    print_string("toka rivi\n", 10);

    for (i=0; i < 100; i++) {
    print_string("ABC", 3);
    }

    draw_rectangle(150, 100, 100, 100);
  
    /*
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
        */


    //usableMem = usableMem / (1024*1024);
    //char *sizeBuffer[100];
    //itoa(usableMem, sizeBuffer, 10);


    setup_interrupts();
    __asm__ volatile ("sti");

    while (1) {}
}

void flush_tlb_all(uint64_t new_cr3) {
    __asm__ volatile(
        "mov %0, %%cr3"  // Load new page table address into CR3
        :
        : "r" (new_cr3)
        : "memory"
    );
}

// identity maps lowest few megabytes and 1mb at addr
void new_page_table(uint32_t addr) {
    for (int i = 0; i < 512; i++) {
        pml4[i] = 0;
        pud[i] = 0;
        pmd[i] = 0;
        pmd2[i] = 0;
    }

    uint32_t offset1 = ((addr >> 39) & 0x1ff);
    uint32_t offset2 = ((addr >> 30) & 0x1ff);
    uint32_t offset3 = ((addr >> 21) & 0x1ff);

    pml4[0] = ((uint64_t)pud & 0xFFFFFFFFFF000) | 3;
    pud[0] = ((uint64_t)pmd & 0xFFFFFFFFFF000) | 3;
    pud[offset2] = ((uint64_t)pmd2 & 0xFFFFFFFFFF000) | 3;
    pmd2[offset3] = ((uint64_t) addr) | 0b10000011;
    for (int i = 0; i < 20; i++) { 
        pmd[i]  = (i * 0x200000) | 0b10000011;
    }
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