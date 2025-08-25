#include <stdint.h>
#include <stdbool.h>

void memcpy(uint8_t* src, uint8_t* dest, uint32_t len);
void memset(uint8_t* dest, uint8_t val, uint32_t len);
bool memcmp(uint8_t* first, uint8_t* second, uint32_t len);
