#include <libc/mem.h>

void memcpy(uint8_t* src, uint8_t* dest, uint32_t len) {
    for (uint32_t index = 0; index < len; ++index) {
        *(dest + index) = *(src + index);
    }
}

void memset(uint8_t* dest, uint8_t val, uint32_t len) {
    for (uint32_t index = 0; index < len; ++index) {
        *(dest + index) = val;
    }
}