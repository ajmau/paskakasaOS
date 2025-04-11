#include <portio.h>

#define PIC1		0x20		
#define PIC2		0xA0		
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
#define ICW_1 0x11
#define ICW_2_M 0x20
#define ICW_2_S 0x28
#define ICW_3_M 0x2
#define ICW_3_S 0x4
#define ICW_4 0x01

/*
#define SPURIOUS_INTERRUPT_VECTOR 0xf0
#define EOI                       0xB0
#define TIMER_LVT                 0x320
#define LOCAL_APIC_ID             0x20 

#define APIC_ENABLE (1 << 8)

#define APIC_LVT_TIMER 0x320
#define APIC_TIMER_DIV 0x3e0
#define APIC_TIMER_INIT_COUNT 0x380


#define IA32_APIC_BASE_MSR 0x1B
#define APIC_BASE_ENABLE  (1 << 11)
#define APIC_BASE_ADDRESS 0xFEE00000

static inline void write_msr(uint32_t msr, uint64_t value) {
    uint32_t low = value;
    uint32_t high = value >> 32;
    __asm__ volatile ("wrmsr" :: "c"(msr), "a"(low), "d"(high));
}

static inline uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

void init_apic(uint64_t *local_apic, uint64_t *io_apic)
{
    // disable  PIC
    outb(PIC1_COMMAND, ICW_1);
    outb(PIC2_COMMAND, ICW_1);
    outb(PIC1_DATA, ICW_2_M);
    outb(PIC2_DATA, ICW_2_S);
    outb(PIC1_DATA, ICW_3_M);
    outb(PIC2_DATA, ICW_3_S);
    outb(PIC1_DATA, ICW_4);
    outb(PIC2_DATA, ICW_4);
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    map_page((uint64_t)local_apic, (uint64_t)local_apic);
    map_page((uint64_t)local_apic+0x1000, (uint64_t)local_apic+0x1000);
    map_page((uint64_t)local_apic+0x2000, (uint64_t)local_apic+0x2000);
    map_page((uint64_t)local_apic+0x3000, (uint64_t)local_apic+0x3000);

    uint64_t apic_base = read_msr(IA32_APIC_BASE_MSR);
    apic_base |= APIC_BASE_ENABLE;
    write_msr(IA32_APIC_BASE_MSR, apic_base);

    volatile uint32_t* lapic = (volatile uint32_t*)local_apic;
    lapic[SPURIOUS_INTERRUPT_VECTOR / 4] = 0x1FF;
    lapic[APIC_TIMER_DIV / 4] = 0x3;
    lapic[APIC_TIMER_INIT_COUNT / 4] = 0xFFFFFFFF;
    lapic[TIMER_LVT / 4] = 32 | (1 << 17);

    __asm__ volatile("sti");


   // *(local_apic+SPURIOUS_INTERRUPT_VECTOR) = 0x1FF;

   // *(local_apic+APIC_TIMER_DIV) = 0x3;
   // *(local_apic+APIC_TIMER_INIT_COUNT) = 0xFFFFFFFF;
}
*/

#include <stdint.h>
#include <vmm.h>
#include <apic.h>



static inline void write_msr(uint32_t msr, uint64_t value) {
    uint32_t low = (uint32_t)value;
    uint32_t high = (uint32_t)(value >> 32);
    __asm__ volatile ("wrmsr" :: "c"(msr), "a"(low), "d"(high));
}

static inline uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

void init_apic(uint64_t* a,  uint64_t*b) {

    // disable  PIC

    // init commands
    outb(PIC1_COMMAND, ICW_1);
    outb(PIC2_COMMAND, ICW_1);

    // specify IDT entries above 32
    outb(PIC1_DATA, ICW_2_M);
    outb(PIC2_DATA, ICW_2_S);
     
    // configure master and slave
    outb(PIC1_DATA, ICW_3_M);
    outb(PIC2_DATA, ICW_3_S);

    // 8086 mode
    outb(PIC1_DATA, ICW_4);
    outb(PIC2_DATA, ICW_4);

    // Mask interrupts
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    // Enable APIC via MSR
//    map_page(apic_base, apic_base);
//    apic_base |= APIC_BASE_ENABLE;

    // this MSR contains APICs address and is used to enable APIC
    map_page((uint64_t)a, (uint64_t)a);
    write_msr(IA32_APIC_BASE_MSR, (uint64_t)a | APIC_BASE_ENABLE);
    uint64_t apic_base = read_msr(IA32_APIC_BASE_MSR);

    // Enable APIC and set spurious vector (bit 8 enables APIC)
    APIC_REG(APIC_SVR) = 0xFF | (1 << 8);


    // Set timer divide config: divide by 16
    APIC_REG(APIC_TIMER_DIV) = 0x3;
    // Set LVT Timer: vector = 0x20, mode = periodic (bit 17)
    APIC_REG(APIC_LVT_TIMER) = 0x20 | (1 << 17);
    // Set initial count (frequency control)
    APIC_REG(APIC_TIMER_INIT_COUNT) = 100000;



    // Unmask interrupts globally
    __asm__ volatile ("sti");
}