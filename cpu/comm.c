#include <arch/x86/cpu/comm.h>

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("out %%al, %%dx" :: "a"(val), "d"(port));
}

void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("out %%ax, %%dx" :: "a"(val), "d"(port));
}

void outl(uint16_t port, uint32_t val) {
    __asm__ volatile ("out %%eax, %%dx" :: "a"(val), "d"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("in %%dx, %%al" : "=a"(ret) : "d"(port));
    return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("in %%dx, %%ax" : "=a"(ret) : "d"(port));
    return ret;
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm__ volatile ("in %%dx, %%eax" : "=a"(ret) : "d"(port));
    return ret;
}