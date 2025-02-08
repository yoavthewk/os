#pragma once
#include <stdint.h>

#define TOTAL_PAGES 4096
#define PAGE_SHIFT 12
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define NULL (void*)(0)

typedef enum {
    FREE = 0,
    ALLOCATED = 1,
    ALLOCATED_START = 2,
    ALLOCATED_END = 3,
} bmap_entry_t;

void init_pmm(void);

void* kalloc_pg(const uint32_t num);

void kfree_pg(void* pg);

uint8_t* get_closest_page(void* addr);

uint8_t* get_closest_page_down(void* addr);

