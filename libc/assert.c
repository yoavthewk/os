#include <kernel/common.h>
#include <kernel/vga.h>

void __assert_failed(const char* assert, const char* file, const char* function, uint32_t line) {
    kprintfln("Assertion failed: %s\nFile: %s\nFunction: %s\nLine: %d\n", assert, file, function, line);

    kpanic("Sadge no printf here");
}