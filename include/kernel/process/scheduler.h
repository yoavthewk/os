#pragma once
#include <stdint.h>
#include <arch/x86/cpu/int.h>

void schedule(irq_frame_t* frame);
void init_scheduler(void);
