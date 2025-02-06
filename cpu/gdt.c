#include <arch/x86/cpu/gdt.h>
#include <arch/x86/cpu/tts.h>

gdt_entry_t gdt[GDT_ENTRIES];
gdt_reg_t gdt_reg;

// Written in gdt.asm
extern void gdt_flush(uint32_t __gdt_reg);

void set_gdt_entry(
    const uint8_t index, const uint32_t base, 
    const uint32_t limit, const uint8_t access,
    const uint8_t granularity
) {
    // segment base is split due to backwards-compatability.
    gdt[index].base_low = (base & 0xFFFF);
    gdt[index].base_mid = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].granularity = (limit >> 16) & 0x0F;

    // set up granularity and access flags.
    gdt[index].granularity |= granularity & 0xF0;
    gdt[index].access = access;
}

void init_gdt(void) {
    // initiate the gdt register.
    gdt_reg.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
    gdt_reg.address = (uint32_t)&gdt;

    // ---- initiate each of the segments. ---- 
    // NULL segment.
    set_gdt_entry(0, 0, 0, 0, 0);

    // Code
    set_gdt_entry(
        1,
        0,
        X86_MAX,
        GDT_PRESENT | GDT_KERNEL | GDT_CODE | GDT_RW,
        GDT_GRANULARITY | GDT_OPERAND_SIZE
    );

    // Data - same as code, but we set the access byte to data and remove R/W.
    set_gdt_entry(
        2,
        0,
        X86_MAX,
        GDT_PRESENT | GDT_KERNEL | GDT_DATA,
        GDT_GRANULARITY | GDT_OPERAND_SIZE
    );

    // User space code.
    set_gdt_entry(
        3,
        0,
        X86_MAX,
        GDT_PRESENT | GDT_USER | GDT_CODE | GDT_RW,
        GDT_GRANULARITY | GDT_OPERAND_SIZE
    );

    // User space data.
    set_gdt_entry(
        4,
        0,
        X86_MAX,
        GDT_PRESENT | GDT_USER | GDT_DATA,
        GDT_GRANULARITY | GDT_OPERAND_SIZE
    );

    // Initialize TSS.
    init_tss(5, 0x10);

    // lgdt the new gdt reg.
    gdt_flush((uint32_t)&gdt_reg);

    // load the new TSS.
    tss_flush();
}
