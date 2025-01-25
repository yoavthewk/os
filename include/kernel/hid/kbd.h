#pragma once
#include <stdint.h>
#include <stdbool.h>

#define KB_IRQ_VECTOR 0x01
#define KB_IDT_ENTRY 0x21

void init_ps2_kb(void);
void kb_getline(char* input);