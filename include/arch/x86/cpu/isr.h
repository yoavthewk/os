#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <arch/x86/cpu/int.h>

__attribute__ ((noreturn))
void isr_handler(isr_frame_t* frame);