#include <stdint.h>
#include <kernel/vga.h>
#include <kernel/io/hid/kbd.h>
#include <kernel/io/timer/timer.h>
#include <libc/string.h>
#include <arch/x86/cpu/idt.h>
#include <arch/x86/cpu/gdt.h>
#include <arch/x86/8259/pic.h>
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/kmm.h>
#include <kernel/acpi/acpi.h>
#include <kernel/process/scheduler.h>

#if defined(__linux__)
#error "This was not compiled using a cross-compiler."
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler."
#endif

void __interrupts_init(void) {
    idt_assemble();
    pic_init();
}

void kmain(void) {
    __interrupts_init();
    init_ps2_kb();  

    clear_terminal();

    kprint("[kmain] INITIALIZING PMM...\n");
    init_pmm();
    kprint("[kmain] DONE INITIALIZING PMM...\n");
    kprint("[kmain] INITIALIZING VMM...\n");
    init_vmm();
    kprint("[kmain] DONE INITIALIZING VMM...\n");
    kprint("[kmain] INITIALIZING KHEAP...\n");
    init_kmm();
    kprint("[kmain] DONE INITIALIZING HEAP...\n");
    init_gdt();
    kprint("Welcome!\n");
    init_scheduler();
    init_pit();

    char* line = (char*)kmalloc(256);
    while(0 != strcmp("END", line)) {
        kprint("< ");
        kb_getline(line);
        if (strcmp("END", line)) {
            kprint("lmfao.. I wish :(\n");
        }
    }

    while(1);
}