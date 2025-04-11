#include <stdint.h>
#include <acpi.h>
#include <text_terminal.h>
#include <mem.h>
#include <vmm.h>

#define REGION_START 0x000E0000
#define REGION_END   0x00100000

apic_t apic;

int verify_checksum(uint8_t* table, size_t length) {
    uint8_t sum = 0;
    for (size_t i = 0; i < length; ++i) {
        sum += table[i];
    }
    return sum;
}

XSDP_t *find_xsdp()
{
    int xsdpFound = 0;

    // scan memory region for string "RSD PTR "
    for (uint64_t addr = REGION_START; addr < REGION_END; addr += 16) {
        if (strcmp((char*)addr, "RSD PTR ", 8) == 0) {
            xsdpFound=1;

            // TODO: checksum verification missing
            return (XSDP_t*)addr;
            
        }
    }

    return (XSDP_t*)0x0;
}

void parse_rsdt(RSDT_t *rsdt)
{
    for (uint32_t sdt_index = 0; sdt_index < 25; sdt_index++) {
        RSDT_t *table = (RSDT_t*)rsdt->sdt_addresses[sdt_index];

        if (strcmp(table->sdt_header.signature, "APIC", 4) == 0) {
            MADT_t *madt = (MADT_t*)table;

            uint8_t *entries = madt->records;
            uint8_t* end = (uint8_t*)madt+madt->sdt_header.length;

            apic.local_apic = (uint64_t*)madt->APIC_address;

            int x = 0;
            while (entries < end) {

                MADT_header_t *entry_header = (MADT_header_t*) entries;

                switch (entry_header->type) {
                case 0:

                    local_apic_t *localapic = (local_apic_t*)entries;
                    print("LOCAL APIC ID: ");

                    if (localapic->apicID == 0) {
                        print("0\n");
                    } else if (localapic->apicID == 1) {
                        print("1\n");
                    } else {
                        print_hex((uint64_t)localapic->apicID);
                    }

                    break;

                case 1:
                    io_apic_t *ioapic = (io_apic_t*)entries;
                    apic.io_apic = (uint64_t*)ioapic->address;
                    break;
                }

                entries+=entry_header->length;

                x++;
            }

            break;

        }
    }
}

uint64_t swap_upper_lower(uint64_t value) {
    // Mask the upper 32 bits and shift them to the lower 32 bits
    uint32_t lower = (uint32_t)(value & 0xFFFFFFFF);
    uint32_t upper = (uint32_t)((value >> 32) & 0xFFFFFFFF);
    
    // Shift upper to the lower 32 bits and lower to the upper 32 bits
    return ((uint64_t)lower << 32) | upper;
}

void parse_xsdt(XSDT_t *xsdt)
{

    if (verify_checksum((uint8_t*)&xsdt->sdt_header, xsdt->sdt_header.length) == 0) {
        print("checksum verification successful\n");
    } else {
        print("checksum verification failed\n");
    }
    
    uint64_t *entries = xsdt->sdt_addresses;
    uint32_t entry_count = (xsdt->sdt_header.length - sizeof(SDTHeader_t)) / sizeof(uint64_t);

    for (uint32_t sdt_index = 0; sdt_index < entry_count; sdt_index++) {
        print("parse_xsdt loop: ");
        print_hex(sdt_index);

        XSDT_t *table = (XSDT_t*)swap_upper_lower((xsdt->sdt_addresses[sdt_index]));
        map_page((uint64_t)table, (uint64_t)table);

        print("table: ");
        print(table->sdt_header.signature);


       if (strcmp(table->sdt_header.signature, "APIC", 4) == 0) {

            MADT_t *madt = (MADT_t*)table;
            uint8_t *entries = madt->records;
            uint8_t* end = (uint8_t*)madt+madt->sdt_header.length;

            apic.local_apic = (uint64_t*)madt->APIC_address;

            while (entries < end) {

                MADT_header_t *entry_header = (MADT_header_t*) entries;

                switch (entry_header->type) {
                case 0:

                    local_apic_t *localapic = (local_apic_t*)entries;
                    print("LOCAL APIC ID: ");
                    if (localapic->apicID == 0) {
                        print("0\n");
                    } else if (localapic->apicID == 1) {
                        print("1\n");
                    } else {
                        print_hex((uint64_t)localapic->apicID);
                    }

                    break;

                case 1:
                    io_apic_t *ioapic = (io_apic_t*)entries;
                    print("IO APIC address: ");
                    print_hex(ioapic->address);
                    apic.io_apic = (uint64_t*)ioapic->address;
                    break;
                }

                entries+=entry_header->length;
            }

            break;

        } 
    }
}

void parse_acpi()
{
    // System descriptor pointer.
    XSDP_t *xsdp = find_xsdp();

    if (xsdp == 0x0) {
        print("Root system descriptor pointer not found!\n");
        asm("hlt");
    }

    // System descriptor table 
//
    if (xsdp->revision >= 2 && xsdp->xsdt_address != 0) {
        // acpi 2.0
        print("acpi version 2\n");
        map_page((uint64_t)xsdp->xsdt_address, (uint64_t)xsdp->xsdt_address);
        print("Mapped xsdt\n");
        parse_xsdt((XSDT_t*)xsdp->xsdt_address);
    } else {
        print("acpi version 1\n");
        map_page((uint64_t)xsdp->rsdt_address, (uint64_t)xsdp->rsdt_address);
        parse_rsdt((RSDT_t*)xsdp->rsdt_address);
    }

}

apic_t* get_apic()
{
    return &apic;
}