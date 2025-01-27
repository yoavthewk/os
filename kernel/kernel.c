#include <stdint.h>
#include <kernel/vga.h>
#include <kernel/hid/kbd.h>
#include <libc/string.h>
#include <arch/x86/cpu/idt.h>
#include <arch/x86/8259/pic.h>

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
    kprint("Welcome!\n");

    char line[256] = { 0 };
    while(0 != strcmp("END", line)) {
        kprint("< ");
        kb_getline(line);
        if (strcmp("END", line)) {
            kprint("lmfao.. I wish :(\n");
        }
    }

    kprint("Goodbye, love you boo.");

    while(1);
}