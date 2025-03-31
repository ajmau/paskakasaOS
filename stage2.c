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

#define PARTITION_START 0x800

uint16_t read2bytes(uint8_t *bytes);   
uint32_t read4bytes(uint8_t *bytes);   
uint64_t read8bytes(uint8_t *bytes);   
static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline uint16_t inw(uint16_t port);
//void read_data(uint8_t *, uint8_t, uint8_t, uint8_t);
void read_data(uint8_t *, uint32_t);
void log(char *msg);

// bios parameter block
struct BPB {
    uint8_t firstBytes[3];
    uint8_t OEM_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserverd_sectors;
    uint8_t number_of_fats;
    uint16_t number_of_roots;
    uint16_t number_of_lvolume_sectors;
    uint8_t media_desc_type;
    uint16_t notused;
    uint16_t sectors_per_track;
    uint16_t heads_num;
    uint32_t hidden_sectors_num;
    uint32_t lsectors_count;
    uint32_t sectors_per_fat;
    uint16_t flags_epb;
    uint16_t version;
    uint32_t root_directory_cluster_number;
    uint16_t fsinfo_sector_num;

};

struct BPB bpb;

void parse_bpb(uint8_t *bytes, struct BPB *bpb);
__attribute__((noreturn))
void loader_main() {

    char *vidmem = (char*)0xb8000;
    *vidmem = 'B';
    log("mitäs nyt sitten");

    uint8_t data[512];
    uint8_t rootdir[512];

    int i = 0;
    for (i = 0; i < 512; i++) {
        data[i] = 0;
    }

    read_data(&data, PARTITION_START);

    parse_bpb(&data, &bpb);

    read_data(&rootdir, PARTITION_START + bpb.reserverd_sectors + (bpb.sectors_per_fat * bpb.number_of_fats));

    char filename[26];
    i = 0;
    for (i=0; i < 26; i++) {
        filename[i] = 0;
    }
    char filedata[512];
    i = 0;
    uint32_t fileloc;
    for (i = 0; i < 512; i+=32) {
        uint32_t entry = rootdir[i] | rootdir[i+1] << 8 | rootdir[2] << 16 | rootdir[3] << 24;
        if (rootdir[i] == 0) {
            break;
        }

        if (rootdir[i+11] = 0x10) {
            log("Found directory");
        }

        if (rootdir[i] == 0xE5) {
            break;
        }

        if (rootdir[i] == 0x85) {
            asm volatile ("hlt");
        }

        if (rootdir[i+11] == 0x0f) {
            /*
            int x=0;
            for(x=0; x<10; x++) {
                filename[x]= rootdir[i+1+x];
            }
            */

        } else {
            int x;
            for (x = 0; x < 13; x++) {
                if (rootdir[i+x] == 0x20) {
                    break;
                }
                filename[x] = rootdir[i+x];
            }
            filename[x] = '.';
            filename[x+1] = rootdir[i+x+2];
            filename[x+2] = rootdir[i+x+3];
            filename[x+3] = rootdir[i+x+4];
            filename[11]  = '\n';

            uint8_t cluster = rootdir[i+26];
            uint8_t size = rootdir[i+28];

            fileloc = PARTITION_START + bpb.reserverd_sectors + (bpb.number_of_fats * bpb.sectors_per_fat) + (cluster - 2) * bpb.sectors_per_cluster;

        }



        asm volatile ("nop");
    }

    read_data(&filedata, fileloc);

    int length = 13;
    i = 0;
    while (i < length) {
        *vidmem++ = filename[i];
        *vidmem++ = 0x0e;
        i++;
    }

    *vidmem++ = ':';
    *vidmem++ = 0x0e;
    vidmem+=144;
    *vidmem = 'X';
    i=0;
    while (i < length) {
        if (filedata[i] != '\n') {
        *vidmem++ = filedata[i];
        *vidmem++ = 0xd;
        i++;
        }
    }

    log("Filename: ");
    log(filename);
    //read_data(&rootdir, 0x0806 + bpb.reserverd_sectors + (bpb.sectors_per_fat * 2));
    uint8_t status = inb(0x1F7);
    while (1) {
        asm volatile ("hlt");
    }
}


void parse_bpb(uint8_t *bytes, struct BPB *bpb) {
    bpb->firstBytes[0] = *bytes++;
    bpb->firstBytes[1] = *bytes++;
    bpb->firstBytes[2] = *bytes++;

    int i;
    for (i=0; i < 8; i++) {
        bpb->OEM_identifier[i] = *bytes++;
    }
    bpb->bytes_per_sector = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->sectors_per_cluster = *bytes;
    bytes++;
    bpb->reserverd_sectors = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->number_of_fats = *bytes;
    bytes++;
    bpb->number_of_roots = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->number_of_lvolume_sectors = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->media_desc_type = *bytes;
    bytes++;
    bpb->notused = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->sectors_per_track = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->heads_num = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->hidden_sectors_num = (*bytes) | *(bytes+1) << 8 | *(bytes+2) << 16 | *(bytes+3) << 24;
    bytes+=4;
    bpb->lsectors_count     = (*bytes) | *(bytes+1) << 8 | *(bytes+2) << 16 | *(bytes+3) << 24;
    bytes+=4;
    bpb->sectors_per_fat    = *bytes | *(bytes+1) << 8 | *(bytes+2) << 16 | *(bytes+3) << 24;
    bytes+=4;
    bpb->flags_epb = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->version = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
    bpb->root_directory_cluster_number = (*bytes) | *(bytes+1) << 8 | *(bytes+2) << 16 | *(bytes+3) << 24;
    bytes+=4;
    bpb->fsinfo_sector_num = (*bytes) | *(bytes+1) << 8;
    bytes+=2;
}

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "dN"(port));
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


void read_data(uint8_t *data, uint32_t lba)
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
        val = inw(DATA);
        data[i] = val & 0xFF;
        data[i+1] = (val & 0xFF00) >> 8;
    }

    outb(FEATURES, 0x0);

}