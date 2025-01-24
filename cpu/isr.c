#include <kernel/vga.h>
#include <libc/string.h>
#include <arch/x86/cpu/isr.h>
#include <arch/x86/cpu/idt.h>

void isr_handler(uint8_t exception_number) {
    char exception[3] = { 0 };
    itoa((uint32_t)exception_number, exception);
    kprint("Got exception: ");
    kprint(exception);
    kprint("\n");
    __asm__ volatile ("cli; hlt");
    while(1);
}
