#ifndef ACPI_H
#define ACPI_H

typedef struct {
 char signature[8];
 uint8_t checksum;
 char OEMID[6];
 uint8_t revision;
 uint32_t rsdt_address;      // deprecated since version 2.0

 uint32_t length;
 uint64_t xsdt_address;
 uint8_t extended_checksum;
 uint8_t reserved[3];
} XSDP_t __attribute__ ((packed));

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t creatorID;
    uint32_t creator_revision;
} SDTHeader_t __attribute__ ((packed));

typedef struct {
    SDTHeader_t sdt_header;
    uint32_t sdt_addresses[];
} RSDT_t __attribute__ ((packed)); 

typedef struct {
    SDTHeader_t sdt_header;
    uint64_t sdt_addresses[];
}  XSDT_t __attribute__ ((packed));

typedef struct {
    SDTHeader_t sdt_header;
    uint32_t APIC_address;
    uint32_t flags;
    uint8_t records[];
} MADT_t __attribute__ ((packed));

typedef struct {
    uint8_t type;
    uint8_t length;
} MADT_header_t __attribute__ ((packed));

typedef struct {
    uint8_t type; 
    uint8_t length;
    uint8_t apicID;
    uint8_t apicVersion;
    uint32_t flags;
} local_apic_t __attribute__ ((packed));

typedef struct {
    uint8_t type;
    uint8_t length;
    uint8_t id;
    uint8_t reserved;
    uint32_t address;
    uint32_t global_system_interrupt_base;
} io_apic_t __attribute__ ((packed));


typedef struct {
    uint64_t *local_apic;
    uint64_t *io_apic;
} apic_t;

void parse_acpi();
apic_t *get_apic();

#endif