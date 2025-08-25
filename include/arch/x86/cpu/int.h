#pragma once
#include <stdint.h>
#include <stdbool.h>

// This should be extended later on
// for easier debugging.
typedef struct {
    // segment registers.
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    // registers pusha.
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
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
    // segment registers.
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    // registers pusha.
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    // pushed in the stub.
    uint32_t int_no;
    uint32_t irq_vector;
    // pushed by processor automatically.
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t useresp;
    uint32_t ss;
} __attribute__((packed)) irq_frame_t;

typedef void (*irq_handler_t)(irq_frame_t*);
