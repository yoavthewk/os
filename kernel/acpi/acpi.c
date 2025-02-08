#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <libc/assert.h>
#include <kernel/acpi/acpi.h>
#include <kernel/acpi/madt.h>
#include <kernel/vga.h>
#include <libc/mem.h>

/*
    CURRENTLY UNUSED - 
    Should be used if I want to implement a multi-processor feature.
*/

typedef void(*parse_fn_t)(void*);
rsdt_t* rsdt = NULL;

void __print_acpiv(rsdp_t* rsdp) {
    if (rsdp->revision) {
        kprintln("ACPI Version is 2.0+.");
    }
    else {
        kprintln("ACPI Version is 1.0.");
    }
}

bool __validate_rsdp_csum(rsdp_t* rsdp) {
    uint32_t sum = 0;
    for (uint32_t index = 0; index < sizeof(rsdp_t); ++index) {
        sum += *((uint8_t*)rsdp + index);
    }

    return 0 == (uint8_t)sum;
}

bool __validate_csum(acpisdt_header_t* header) {
    uint32_t sum = 0;
    for (uint32_t index = 0; index < sizeof(acpisdt_header_t); ++index) {
        sum += *((uint8_t*)header + index);
    }

    return 0 == sum;
}

rsdp_t* __verify_rsdp(rsdp_t* rsdp) {
    __print_acpiv(rsdp);

    if (__validate_rsdp_csum(rsdp)) {
        return rsdp;
    }
    return NULL;
}

rsdp_t* __locate_rsdp(void) {
    uint32_t max_index = RSDP_REG_END - RSDP_REG_START - RSDP_SIG_LEN;
    for (uint32_t index = 0; index < max_index; ++index) {
        const uint8_t* cur = (uint8_t*)RSDP_REG_START + index;
        if (memcmp(cur, RSDP_SIG, RSDP_SIG_LEN)) {
            return __verify_rsdp((rsdp_t*)cur);
        }
    }

    return NULL;
}

void __parse_table(const char* sig, parse_fn_t parse) {
    assert(rsdt != NULL);

    const uint8_t* rsdt_ptr_page = get_closest_page_down((uint8_t*)rsdt);
    uint32_t entries = (rsdt->header.len - sizeof(rsdt->header)) / sizeof(uint32_t);
    for (uint32_t entry = 0; entry < entries; ++entry) {
        // same as before.
        uint8_t* table_page = get_closest_page_down((void*)rsdt->entries[entry]);
        if (table_page != rsdt_ptr_page) {
            vmm_map(table_page, table_page);
        }

        acpisdt_header_t* table_header = (acpisdt_header_t*)rsdt->entries[entry];
        if (memcmp((uint8_t*)table_header->sig, (uint8_t*)sig, 4)) {
            parse((void*)table_header);
            if (table_page != rsdt_ptr_page) {
                vmm_unmap(table_page);
            }
            return;
        } 

        if (table_page != rsdt_ptr_page) {
            vmm_unmap(table_page);
        }
    }

    kprintfln("Could not find requested APIC table... Panicing...");
    kpanic();
}

void parse_acpi(void) {
    rsdp_t* rsdp = __locate_rsdp();
    if (NULL == rsdp) {
        kprintln("Failed to find or verify the RSDP table!");
    }
    kprintfln("Found rsdp: %x", rsdp);

    // Identity map the page in which the table resides.
    // note: we DO NOT care about the result of vmm_map, we just need it to be mapped.
    uint8_t* rsdt_ptr_page = get_closest_page_down(rsdp->rsdt_ptr);
    vmm_map(rsdt_ptr_page, rsdt_ptr_page);
    rsdt = (rsdt_t*)rsdp->rsdt_ptr;

    // Parse MADT table; this table contains info about the processors
    // and interrupt controller system which is necessary for multi-cpu spport.
    __parse_table(MADT_SIG, parse_madt);

    // Remove the identity map.
    vmm_unmap(rsdt_ptr_page);
}