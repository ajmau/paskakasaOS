#include <stddef.h>
#include <fat32.h>
#include <mem.h>
#include <ata_pio.h>

#define BUFFER_SIZE 512
#define ENTRY_UNUSED 0xe5
#define ENTRY_LFN 0x0f
#define ENTRY_DIR 0x10

bpb_t bpb;

// buffer for reading disk
uint8_t buffer[BUFFER_SIZE] = {0};

//lfn_t lfnlist[5] = {0};
//sfn_t sfnlist[5] = {0};

lfn_t lfn;
sfn_t sfn;


typedef struct file {
    char name[56];
    char short_name[11];
    uint32_t first_cluster;
    char size[4];
} file_t;

file_t files[5]  = {0};

uint32_t read_nextcluster(uint32_t *fattable, uint32_t cluster)
{
    uint32_t next = fattable[cluster];
    if (next == 0xfffffff) {
        return 0;
    }
    else {
        return next;
    }
}

void read_filename_from_lfn(file_t *file, lfn_t *lfn)
{
    memmove(file->name, lfn->start, 10);
    memmove((file->name+10), lfn->mid, 12);
    memmove((file->name+22), lfn->end, 4);
}

uint8_t process_directory_area()
{
    read_sector(buffer, PARTITION_START + bpb.reserverd_sectors + (bpb.sectors_per_fat * bpb.number_of_fats));

    char filename[20] = {0};

    size_t fileIndex = 0;
    size_t i;

    uint8_t foundfile = 0;
    for (i  = 0; i < BUFFER_SIZE; i+=32) {
        if (buffer[i] == 0)  {
            break;
        }

        if (buffer[i] == ENTRY_UNUSED) {
            continue;
        }

        if (buffer[i+11] == ENTRY_LFN) {
            memmove(&lfn, &buffer[i], sizeof(lfn_t));
            read_filename_from_lfn(&files[0], &lfn);
            foundfile = 1;
            continue;
        }

        if (buffer[i+11] == ENTRY_DIR) {
            continue;
        }

        if (foundfile == 1)  {
            memmove(&sfn, &buffer[i], sizeof(sfn_t));
            files[fileIndex].first_cluster  = sfn.cluster_low_bits | (sfn.cluster_high_bits << 16) & 0x00FF;
            memmove(files[fileIndex].size, sfn.size, 4);
            memmove(files[fileIndex].short_name, sfn.name, 11);
            fileIndex++;
            foundfile = 0;
        }
    }

    return 0;

}

void init_fat()
{

    // read bios parameter block from start of partition
    read_sector(buffer, PARTITION_START);
    memmove((uint8_t*)&bpb, &buffer, sizeof(bpb_t));
    //parse_bpb(&buffer, &bpb);

    process_directory_area();
}

uint8_t read_file(char *path, void *address)
{
    uint32_t fat[128] = {0};

    size_t i = 0;
    int  cluster=0;
    for (i = 0; i < 5; i++) {
        if (strcmp(files[i].short_name, path, 11) == 0) {
            cluster = files[i].first_cluster;
        }
    }

    // file not found
    if (cluster == 0) {
        return 1;
    }

    // read fat table
    read_sector(&fat, PARTITION_START + bpb.reserverd_sectors);


    uint32_t clusterdataLBA = PARTITION_START + bpb.reserverd_sectors + (bpb.number_of_fats * bpb.sectors_per_fat) + (cluster - 2) * bpb.sectors_per_cluster;

    uint32_t next  =  cluster;
    int  memIndex = 1;

    read_sector((uint8_t*)address, clusterdataLBA);
    while (1) {
        next = read_nextcluster(&fat, next);
        if (next == 0) {
            break;
        }
        uint32_t clusterdataLBA = PARTITION_START + bpb.reserverd_sectors + (bpb.number_of_fats * bpb.sectors_per_fat) + (next - 2) * bpb.sectors_per_cluster;


        uint64_t dst = (uint64_t)(address + (memIndex*0x200));
        read_sector((uint8_t*)dst, clusterdataLBA);

        memIndex++;
    } 

}