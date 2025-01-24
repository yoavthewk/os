#include <stdint.h>
#include "../drivers/vga.h"
#include "../libc/string/string.h"

#if defined(__linux__)
#error "This was not compiled using a cross-compiler."
#endif

#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler."
#endif

void kmain(void) {
    clear_terminal();
    kprint("Hello, World! :)\nThis is a newline!");
    kprint("\n\n\nAlejandro is a massive loser.");

    for(uint32_t i = 1; i < 100; ++i) {
        char number[4] = {0};
        itoa(i, number);
        kprint(number);
        kprint("\n");
    }
    for (uint32_t i = 0; i < 50; ++i) {
        char number[4] = {0};
        itoa(i, number);
        kprint(number);
    }
    while(1);
}