#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}


// Logs messages into COM1 port.
// Qemu can read these messages with "-serial file:serial.log" option
void log(char *msg) {
    const uint16_t COM1 = 0x3f8;

    while (*msg != '\0') {
        outb(COM1, *msg);
        msg++;
    }
}

void _entry()
{
    char *vidmem = (char*)0xb8000;
    char *msg = "Hello from protected mode";
    int length = 26;

    int i = 0;
    while (i < length) {
        *vidmem = msg[i];
        vidmem++;
        *vidmem = 0x4;
        vidmem++;
        i++;
    }

    log("Log message 1\n");
    log("log message 2\n");
    
}