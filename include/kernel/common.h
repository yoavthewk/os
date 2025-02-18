#pragma once
#include <stdint.h>

static inline void kpanic(void) {
    __asm__ volatile ("cli; hlt");
    while(1);
}
