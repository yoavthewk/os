#pragma once
#include <stdint.h>
#include <stdbool.h>

// This should be extended later on
// for easier debugging.
// I assume paging will make this horrible.
typedef struct {
    // pushed in the stub.
    uint32_t int_no;
    uint32_t err_code;
    // pushed by processor automatically.
    uint32_t eip;
    uint32_t cs;
    uint32_t rflags;
    uint32_t useresp;
    uint32_t ss;
} __attribute__((packed)) isr_frame_t;

typedef struct {
    // pushed in the stub.
    uint32_t int_no;
    uint32_t irq_vector;
    // pushed by processor automatically.
    uint32_t eip;
    uint32_t cs;
    uint32_t rflags;
    uint32_t useresp;
    uint32_t ss;
} __attribute__((packed)) irq_frame_t;

typedef void (*irq_handler_t)(irq_frame_t*);
