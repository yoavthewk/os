#include <kernel/common.h>
#include <kernel/vga.h>

void __assert_failed(const char* assert, const char* file, const char* function, uint32_t line) {
    // TODO: PRINTF, jeez.
    kprint("ASSERTION FAILED: ");
    kprintln(assert);
    kprint("FILE: ");
    kprintln(file);
    kprint("FUNC: ");
    kprintln(function);
    kprint("LINE: ");
    kprintln(line);

    kpanic();
}