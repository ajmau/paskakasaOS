#include <stdint.h>

#define DATA         0x1F0
#define ERROR        0x1F1
#define FEATURES     0x1F1
#define SECTOR_COUNT 0x1F2
#define LBALOW       0x1F3
#define LBAMID       0x1F4
#define LBAHIGH      0x1F5
#define HEAD         0x1F6
#define STATUS       0x1F7
#define COMMAND      0x1F7

#define CONTROL      0x3F6

static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline uint16_t inw(uint16_t port);
void read_data();
void log(char *msg);

__attribute__((noreturn))
void loader_main() {

    char *vidmem = (char*)0xb8000;
    *vidmem = 'B';
    log("mitäs nyt sitten");
    read_data();

    while (1) {
        asm volatile ("hlt");
    }
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ volatile ( "inb %w1, %b0" : "=a"(val) : "Nd"(port) : "memory");
    return val;
}
static inline uint16_t inw(uint16_t port)
{
    uint16_t val;
    __asm__ volatile ( "inw %w1, %w0" : "=a"(val) : "Nd"(port) : "memory");
    return val;
}

void log(char *msg) {
    const uint16_t COM1 = 0x3f8;

    while (*msg != '\0') {
        outb(COM1, *msg);
        msg++;
    }
}


void read_data()
{
    uint8_t sectorcount = 0;

    outb(HEAD, 0xE0); //| 0x0F);
    //outb(HEAD, 0xE0 | ((0 >> 24) & 0x0F));

    inb(DATA);
    inb(DATA);
    inb(DATA);
    inb(DATA);

    //outb(FEATURES, 0x0);
    // 2332
    outb(SECTOR_COUNT, sectorcount);
    outb(LBALOW, 0x1c);
    outb(LBAMID, 0x9);
    outb(LBAHIGH, 0);
    outb(COMMAND, 0x20);

    uint8_t status = inb(STATUS);
    uint16_t data[256];

    int i;
    for (int i = 0; i<256; i++) {
        data[i] = inw(DATA);
    }

    uint8_t error = inb(STATUS);

    outb(FEATURES, 0x0);

}