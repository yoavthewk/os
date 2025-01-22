#include <stdint.h>
#include "../drivers/vga.h"

#if defined(__linux__)
#error "This was not compiled using a cross-compiler."
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler."
#endif

void kmain(void) {
    clear_terminal();
    kprint("Hello, World! :)\nThis is a newline!");
}