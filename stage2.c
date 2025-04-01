#include <stdint.h>
#include <stddef.h>

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

uint32_t read_nextcluster(uint32_t *fattable, uint32_t cluster);
uint16_t read2bytes(uint8_t *bytes);   
uint32_t read4bytes(uint8_t *bytes);   
uint64_t read8bytes(uint8_t *bytes);   
static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline uint16_t inw(uint16_t port);
//void read_data(uint8_t *, uint8_t, uint8_t, uint8_t);
void read_data(uint8_t *, uint32_t);
void log(char *msg);
void* memmove(void* dstptr, const void* srcptr, size_t size);

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

struct sfn {
    char name[11];
    char attributes;
    char reserved;
    char creation_seconds;
    char created_time[2];
    char created_date[2];
    char last_accessed[2];
    uint16_t cluster_high_bits;
    char modification_time[2];
    char modification_date[2];
    uint16_t cluster_low_bits;
    char size[4];
};

struct lfn {
    char orderno;
    char start[10];
    char attribute;
    char long_entry_type;
    char checksum;
    char mid[12];
    char zero1;
    char zero2;
    char end[4];
};  

struct sfn sfn;
struct lfn lfn;

void lfn_fixname(struct lfn *entry);

int clusters = 0;

void parse_bpb(uint8_t *bytes, struct BPB *bpb);
__attribute__((noreturn))
void loader_main(uint32_t memorymap) {

    clusters = 0;

    uint8_t data[512];
    uint32_t fat[128];

    int i = 0;
    for (i = 0; i < 512; i++) {
        data[i] = 0;
    }

    read_data(&data, PARTITION_START);

    parse_bpb(&data, &bpb);

    log("FAT32 BPB OEM: ");
    log(bpb.OEM_identifier);
    log("\n");

    for (i = 0; i < 512; i++) {
        data[i] = 0;
    }
    read_data(&data, PARTITION_START + bpb.reserverd_sectors + (bpb.sectors_per_fat * bpb.number_of_fats));
    read_data(&fat, PARTITION_START + bpb.reserverd_sectors);

    char filename[26];
    i = 0;
    for (i=0; i < 26; i++) {
        filename[i] = 0;
    }
    char filedata[512];
    i = 0;
    uint32_t fileloc;
    int entries = 0;
    for (i = 0; i < 512; i+=32) {
        //uint32_t entry = data[i] | data[i+1] << 8 | data[2] << 16 | data[3] << 24;

        if (data[i] == 0) {
            log("No more files in directory area\n");
            break;
        }

        if (data[i] == 0xe5) {
            log("Unused directory entry\n");
            continue;;
        }

        if (data[i+11] == 0x0f) {
            memmove(&lfn, &data[i], 32);
            log("Long file entry found: ");
            log(lfn.start);
            log(lfn.mid);
            log(lfn.end);
            log("\n");
        } else {
            memmove(&sfn, &data[i], 32);
            log("Short file entry found: ");
            log(sfn.name);
            log("\n");
        }
        int limit = 32;

        entries++;

        if (data[i+11] == 0x10) {
            log("Found directory\n");
        }

        if (data[i] == 0xE5) {
            break;
        }

    }

    uint32_t cluster = sfn.cluster_low_bits | (sfn.cluster_high_bits << 16) & 0x00FF; 
    uint32_t clusterdataLBA = PARTITION_START + bpb.reserverd_sectors + (bpb.number_of_fats * bpb.sectors_per_fat) + (cluster - 2) * bpb.sectors_per_cluster;

    read_data((uint8_t*)0x100000, clusterdataLBA);

    uint32_t next  =  cluster;
    int  memIndex = 1;
    // calculate first cluster
    while (1) {
        next = read_nextcluster(&fat, next);
        if (next == 0) {
            break;
        }
        uint32_t clusterdataLBA = PARTITION_START + bpb.reserverd_sectors + (bpb.number_of_fats * bpb.sectors_per_fat) + (next - 2) * bpb.sectors_per_cluster;


        uint64_t address = 0x100000 + (memIndex*0x200);
        read_data((uint8_t*)address, clusterdataLBA);
        log(filedata);

    memIndex++;
    } 
    __asm__ volatile ("xchg %%bx, %%bx" ::: "bx");
    // Jump to kernel code loaded from disk
    void (*kernel_entry)(uint32_t) = (void (*)(uint32_t))0x100000;
    kernel_entry(memorymap);

    while (1) {
        asm volatile ("hlt");
    }
}

uint32_t read_nextcluster(uint32_t *fattable, uint32_t cluster)
{
    uint32_t next = fattable[cluster];
    if (next == 0xfffffff) {
        return 0;
    }
    else {
        clusters++;
        return next;
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
        if (i == 510) {
            asm volatile ("nop");
        }
        val = inw(DATA);
        data[i] = val & 0xFF;
        data[i+1] = (val & 0xFF00) >> 8;
    }

    outb(FEATURES, 0x0);

}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}
