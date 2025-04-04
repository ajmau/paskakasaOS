#include <stdint.h>
#include <stddef.h>
#include <interrupts.h>
#include <mem.h>
#include <pmm.h>
#include <vmm.h>
#include <vesa.h>
#include <fat32.h>
#include <text_terminal.h>

void log(char *msg);
char * itoa( int value, char * str, int base );
void new_page_table(uint32_t addr);
void flush_tlb_all(uint64_t);
void identity_map_1gb();
__attribute__((aligned(4096))) uint64_t pml4[512];
__attribute__((aligned(4096))) uint64_t pud[512];
__attribute__((aligned(4096))) uint64_t pmd[512];
__attribute__((aligned(4096))) uint64_t pmd2[512];

void start(uint32_t memorymaps, uint32_t vesa)
{
    init_pmm(memorymaps);

    init_vesa(vesa);
    uint32_t fb = get_framebuffer();

    init_paging(fb);
    // read font file from disk
    init_fat();

    uint64_t *mem = (uint64_t*)alloc(10);
    read_file("FONT    PSF", (void*)mem);
    init_text_terminal(mem);

    draw_rectangle(150, 100, 100, 100);
    draw_rectangle(300, 400, 50, 100);

    setup_interrupts();

    char totalmsg[] = "Total memory: ";
    char usablemsg[] = "Usable memory: ";
    uint64_t total = get_total_memory();
    uint64_t usable = get_usable_memory();

    usable = usable / (1024*1024);
    total = total / (1024*1024);

    char sizeBuffer[100];
    itoa(total, sizeBuffer, 10);

    print_string(totalmsg, strlen(totalmsg));
    print_string(sizeBuffer, strlen(sizeBuffer));
    print_string("MB\n", 3);

    itoa(usable, sizeBuffer, 10);
    print_string(usablemsg, strlen(usablemsg));
    print_string(sizeBuffer, strlen(sizeBuffer));
    print_string("MB\n", 3);

    while (1) {}
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