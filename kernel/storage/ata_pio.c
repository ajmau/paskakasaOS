#include <stdint.h>
#include <ata_pio.h>
#include <portio.h>

void read_sector(uint8_t *data, uint32_t lba)
{
    uint8_t sectorcount = 1;
    // slave
    outb(HEAD, 0xE0 | ((lba >> 24) & 0xF));
    
    outb(SECTOR_COUNT, sectorcount);
    outb(LBALOW, lba & 0xFF);
    outb(LBAMID, (lba >> 8) & 0xFF);
    outb(LBAHIGH, (lba >> 16) & 0xFF);
        
    outb(COMMAND, 0x20);

    // wait for disk to be ready
    while (inb(0x1F7) & 0x80);
    while (!(inb(0x1F7) & 0x08));

    int i;
    uint16_t val;
    for (int i = 0; i<512; i+=2) {
        if (i == 510) {
            asm volatile ("nop");
        }
        val = inw(DATA);
        data[i] = val & 0xFF;
        data[i+1] = (val & 0xFF00) >> 8;
    }

    outb(FEATURES, 0x0);

}