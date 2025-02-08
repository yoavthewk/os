#pragma once
#include <stdint.h>

void kpanic(void) {
    __asm__ volatile ("cli; hlt");
    while(1);
}
