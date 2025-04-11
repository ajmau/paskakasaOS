#ifndef APIC_H
#define APIC_H
#include <stdint.h>

#define IA32_APIC_BASE_MSR     0x1B
#define APIC_BASE_ADDRESS      0xFEE00000
#define IO_APIC_BASE_ADDRESS   0xFEC00000
#define APIC_BASE_ENABLE       (1 << 11)

#define APIC_REG(offset)       (*(volatile uint32_t*)((uintptr_t)APIC_BASE_ADDRESS + (offset)))

#define APIC_SVR               0xF0   // Spurious Interrupt Vector Register
#define APIC_EOI               0xB0   // End-of-Interrupt Register
#define APIC_LVT_TIMER         0x320  // Local Vector Table (Timer)
#define APIC_TIMER_INIT_COUNT  0x380  // Initial Count Register
#define APIC_TIMER_CURR_COUNT  0x390  // Current Count Register (RO)
#define APIC_TIMER_DIV         0x3E0  // Divide Configuration Register

#define TIMER_VECTOR           0x20   // Interrupt vector for timer
void init_apic(uint64_t*, uint64_t*);

#endif