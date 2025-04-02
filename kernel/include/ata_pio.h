#ifndef ATA_PIO_H
#define ATA_PIO_H

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

// use partition start directly for now
#define PARTITION_START 0x800

void read_sector(uint8_t *data, uint32_t lba);

#endif