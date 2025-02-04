#pragma once
#include <stdint.h>
#include <stdbool.h>

#define INITIAL_KHEAP_PAGES 0x4
#define KHEAP_START 0xD0000000
#define KHEAP_END 0xE0000000

typedef struct {
    bool free;
    uint32_t size;
    void* next;
} km_block_t; 

void init_kmm(void);

void* kmalloc(const uint32_t size);

void kfree(const void* address);
