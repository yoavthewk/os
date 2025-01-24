#include <stdint.h>
#include <kernel/vga.h>
#include <libc/string.h>
#include <arch/x86/cpu/idt.h>

#if defined(__linux__)
#error "This was not compiled using a cross-compiler."
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler."
#endif

void kmain(void) {
    clear_terminal();
    kprint("Welcome!\n");

    idt_assemble();
    __asm__ volatile("int $0x2");

    while(1);
}