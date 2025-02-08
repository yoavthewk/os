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

bool memcmp(uint8_t* first, uint8_t* second, uint32_t len) {
    for (uint32_t index = 0; index < len; ++index) {
        if (first[index] != second[index]) return false;
    }
    
    return true;
}