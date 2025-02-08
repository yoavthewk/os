#include <kernel/vga.h>
#include <kernel/acpi/madt.h>
#include <kernel/memory/pmm.h>  
#include <libc/assert.h>

void parse_madt(madt_header_t* madt) {
    assert(NULL != madt);

    uint32_t cur = sizeof(madt_header_t);
    while (cur < madt->hdr.len) {
        madt_entry_hdr_t* entry = (madt_entry_hdr_t*)((uint8_t*)madt + cur);

        switch(entry->entry_type) {
            case 0: {
                cpu_lapic_t* cpu_lapic = (cpu_lapic_t*)entry;
                kprintfln("--- ENTRY TYPE 0 ---");
                kprintfln("CPUID: %d", cpu_lapic->cpu_id);
                kprintfln("APICID: %d", cpu_lapic->apic_id);
                break;
            }
            case 1: {
                io_apic_t* io_apic = (io_apic_t*)entry;
                kprintfln("--- ENTRY TYPE 1 ---");
                kprintfln("IOAPICID: %d", io_apic->ioapic_id);
                kprintfln("IOAPICADDR: %x", io_apic->ioapic_addr);
                kprintfln("GSI_BASE: %x", io_apic->gsi_base);
                break;
            }
            case 2: {
                ioapic_iso_t* io_apic_iso = (ioapic_iso_t*)entry;
                kprintfln("--- ENTRY TYPE 2 ---");
                kprintfln("BUS_SRC: %d", io_apic_iso->bus_src);
                kprintfln("IRQ_SOURCE: %d", io_apic_iso->irq_src);
                kprintfln("GSI: %x", io_apic_iso->gsi);
                break;
            }
            case 3: {
                lapic_nmis_t* lapic_nmis = (lapic_nmis_t*)entry;
                kprintfln("--- ENTRY TYPE 3 ---");
                kprintfln("SOURCE: %d", lapic_nmis->nmi_source);
                kprintfln("GSI: %d", lapic_nmis->gsi);
                break;  
            }
            case 4: {
                lapic_nmi_t* lapic_nmi = (lapic_nmi_t*)entry;
                kprintfln("--- ENTRY TYPE 4 ---");
                kprintfln("CPUID: %d", lapic_nmi->cpu_id);
                kprintfln("LINT: %d", lapic_nmi->lint);
                break;
            }
            case 9: {
                lapic_x2_t* lapic_x2 = (lapic_x2_t*)entry;
                kprintfln("--- ENTRY TYPE 9 ---");
                kprintfln("CPUID: %d", lapic_x2->cpu_id);
                kprintfln("ACPI_ID: %d", lapic_x2->acpi_id);
                break;
            }
            default:
                kprintfln("Cannot handle MADT record of type %d", entry->entry_type);
                break;
        }
        
        cur += entry->record_length;
    }
}