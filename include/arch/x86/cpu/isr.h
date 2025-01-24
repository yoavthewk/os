#include <stdint.h>
#include <stdbool.h>

__attribute__ ((noreturn))
void isr_handler(uint8_t exception_number);