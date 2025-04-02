#include <stdint.h>
#include <portio.h>

void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "dN"(port));
    return val;
}

uint16_t inw(uint16_t port)
{
    uint16_t val;
    __asm__ volatile ( "inw %w1, %w0" : "=a"(val) : "Nd"(port) : "memory");
    return val;
}