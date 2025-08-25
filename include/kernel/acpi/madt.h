#pragma once
#include <stdint.h>
#include <kernel/acpi/acpi.h>

#define MADT_SIG "APIC"

typedef struct {
    acpisdt_header_t hdr;
    uint32_t lapic_addr;
    uint32_t flags;
} __attribute__((packed)) madt_header_t;

typedef struct {
    uint8_t entry_type;
    uint8_t record_length;
} __attribute__((packed)) madt_entry_hdr_t;

typedef struct {
    madt_entry_hdr_t hdr;
    uint8_t cpu_id;
    uint8_t apic_id;
    uint32_t flags;
} __attribute__((packed)) cpu_lapic_t;

typedef struct {
    madt_entry_hdr_t hdr;
    uint8_t ioapic_id;
    uint8_t reserved;
    uint32_t ioapic_addr;
    uint32_t gsi_base;
} __attribute__((packed)) io_apic_t;

typedef struct {
    madt_entry_hdr_t hdr;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((packed)) ioapic_iso_t;

typedef struct {
    madt_entry_hdr_t hdr;
    uint8_t nmi_source;
    uint8_t reserved;
    uint16_t flags;
    uint32_t gsi;
} __attribute__((packed)) lapic_nmis_t;

typedef struct {
    madt_entry_hdr_t hdr;
    uint8_t cpu_id;
    uint16_t flags;
    uint8_t lint;
} __attribute__((packed)) lapic_nmi_t;

typedef struct {
    madt_entry_hdr_t hdr;
    uint16_t reserved;
    uint32_t cpu_id;
    uint32_t flags;
    uint32_t acpi_id;
} __attribute__((packed)) lapic_x2_t;

void parse_madt(madt_header_t* madt);
