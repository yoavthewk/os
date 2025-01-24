#include <stdint.h>

#define HIGHER_16(addr) ((uint32_t)(addr) >> 16)
#define LOWER_16(addr) ((uint32_t)(addr) & 0xFFFF)