#include <arch/x86/cpu/tts.h>
#include <arch/x86/cpu/gdt.h>
#include <libc/mem.h>

static tss_entry_t ktts;

void init_tss(uint8_t index, uint32_t ss0) {
    uint32_t base = (uint32_t)&ktts;
    uint32_t limit = base + sizeof(tss_entry_t);

    // add the TTS descriptor to the GDT.
    set_gdt_entry(index, base, limit, GDT_PRESENT | GDT_USER | GDT_EX | GDT_AC, 0x0);

    // initialize the ktts.
    memset(&ktts, 0, sizeof(tss_entry_t));
    ktts.ss0 = ss0;
    ktts.esp0 = 0;
    // 0x08 - Kernel code segment index, 0x03 - Ring 3.
    ktts.cs = 0x08 | 0x03;
    // 0x10 - Kernel data segment index, 0x03 - Ring 3.
    ktts.ds = 0x10 | 0x03;
    ktts.es = 0x10 | 0x03;
    ktts.fs = 0x10 | 0x03;
    ktts.gs = 0x10 | 0x03;
    ktts.ss = 0x10 | 0x03;
    ktts.iomap_base = sizeof(tss_entry_t);
}