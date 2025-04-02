#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>

typedef struct BPB {
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
}__attribute__((packed)) bpb_t;

typedef struct sfn {
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
}__attribute__((packed)) sfn_t;

typedef struct lfn {
    char orderno;
    char start[10];
    char attribute;
    char long_entry_type;
    char checksum;
    char mid[12];
    char zero1;
    char zero2;
    char end[4];
}__attribute__((packed)) lfn_t;

void init_fat();
uint8_t read_file(char*, void*);

#endif