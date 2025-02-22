#include <kernel/memory/pmm.h>
#include <kernel/vga.h>

extern uint8_t phys_kernel_end; // linker variable.

// We want to insert our bmap right after the kernel and make 
// it a reserved space.
// Each bit(byte) in the bmap allocates a page (for now);
// therefore, 4KiB of entries mean an allocation of above a GB.
// Let's give it that for now. 
// @note: kernel end should be aligned to PAGE_SIZE (peek at linker.ld if you don't understand).
uint8_t* __kernel_end = (uint8_t*)&phys_kernel_end;
uint8_t* __start_pg;
uint8_t* __bmap;

#define GET_PG_INDEX(pg) (((uint8_t*)pg - __start_pg) / PAGE_SIZE)

// TODO: turn into bitmap? or maybe something more sophisticated?
//       for this scope, this is okay, but refactor in a later project.

void __null_out_bmap(void) {
    for (uint32_t i = 0; i < TOTAL_PAGES; ++i) {
        __bmap[i] = FREE;
    }
}

void __allocate_region(const uint32_t start_pg, const uint32_t num_pgs) {
    uint32_t i; 

    __bmap[start_pg] = ALLOCATED_START;
    for (i = 1; i < num_pgs; ++i) {
        __bmap[start_pg + i] = ALLOCATED;
    }
    __bmap[start_pg + --i] = ALLOCATED_END;
}

void __free_region(const uint32_t start_pg) {
    uint32_t i;

    for (i = 0; __bmap[start_pg + i] != ALLOCATED_END; ++i) {
        __bmap[start_pg + i] = FREE;
    }
    __bmap[start_pg + i] = FREE;
}

void* __is_free_region(const uint32_t num, const uint32_t cur_pg) {
    uint32_t free_pgs = 0;
    while (FREE == __bmap[cur_pg + free_pgs] && free_pgs < num) {
        ++free_pgs;
    }
    if (num != free_pgs) {
        return NULL;
    }

    // set as allocated in the bmap and return the address.
    __allocate_region(cur_pg, num);
    return __start_pg + PAGE_SIZE * cur_pg;
}

uint8_t* get_closest_page(void* addr) {
    return (((uintptr_t)(addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1));
}

uint8_t* get_closest_page_down(void* addr) {
    return (((uintptr_t)(addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1)) - PAGE_SIZE;
}

void init_pmm(void) {
    __start_pg = get_closest_page(__kernel_end);
    __bmap = __start_pg;
    __null_out_bmap(); // set all pages as free.
    __allocate_region(0, (TOTAL_PAGES / PAGE_SIZE)); // allocate only the pages used by the bmap.
}

// @note: allocates by pages for now
//        later on I'll want a malloc type allocation.
void* kalloc_pg(const uint32_t num) {
    // we want to find consecutive memory to allocate these pages.
    for (uint32_t cur_pg = 0; cur_pg < (TOTAL_PAGES - num); ++cur_pg) {
        void* pg_address = __is_free_region(num, cur_pg);
        
        if (NULL != pg_address) {
            return pg_address;
        }
    }

    return NULL;
}

void kfree_pg(void* pg) {
    __free_region(GET_PG_INDEX(pg));
}

