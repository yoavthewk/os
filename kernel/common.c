#include <kernel/common.h>
#include <kernel/vga.h>

void kpanic(const char* message) {
    kprintln(message);
    __asm__ volatile ("cli; hlt");
    while(1);
}

void cli(void) {
    __asm__ volatile ("cli");
}

void sti(void) {
    __asm__ volatile ("sti");
}
