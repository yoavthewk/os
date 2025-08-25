#pragma once
#include <stdint.h>

/// Frequency divider value (1.193182 MHz).
#define PIT_DIVISOR 1193182
#define PIT_HZ 1193
#define PIT_DATA_0 0x40
#define PIT_CMD 0x43 

/// Bits         Usage
/// 6 and 7 [Select channel]
///     0 0 = Channel 0
///     0 1 = Channel 1
///     1 0 = Channel 2
///     1 1 = Read-back command (8254 only)
/// 4 and 5 [Access mode]
///     0 0 = Latch count value command
///     0 1 = Access mode: lobyte only
///     1 0 = Access mode: hibyte only
///     1 1 = Access mode: lobyte/hibyte
/// 1 to 3  [Operating mode]
///     0 0 0 = Mode 0 (interrupt on terminal count)
///     0 0 1 = Mode 1 (hardware re-triggerable one-shot)
///     0 1 0 = Mode 2 (rate generator)
///     0 1 1 = Mode 3 (square wave generator)
///     1 0 0 = Mode 4 (software triggered strobe)
///     1 0 1 = Mode 5 (hardware triggered strobe)
///     1 1 0 = Mode 2 (rate generator, same as 010b)
///     1 1 1 = Mode 3 (square wave generator, same as 011b)
#define PIT_CONFIGURATION 0x34

// IRQ vector 0 is used for PIT.
#define PIT_IRQ_VECTOR 0
#define PIT_IDT_VECTOR (PIT_IRQ_VECTOR + 0x20)

void init_pit(void);
void timer_phase(const uint32_t hz);
