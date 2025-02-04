/**
 * kheap implementation using free-list.
 */
#include <kernel/memory/kmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>

uint32_t kheap;
mm_zone_t kheap_zone = {
    .base = KHEAP_START,
    .limit = KHEAP_END
};

void __set_block(km_block_t* blk, uint32_t size, bool is_free, km_block_t* next) {
    blk->size = size;
    blk->free = is_free;
    blk->next = next;
}

void* __split_block(km_block_t* blk, const uint32_t size) {
    const uint32_t blksize = blk->size - size;

    km_block_t* new_block = (km_block_t*)((uint8_t*)blk + size + sizeof(km_block_t));
    __set_block(new_block, blksize, true, blk->next);

    blk->size -= new_block->size;
    blk->free = false;
    blk->next = new_block;

    return (void*)((uint8_t*)blk + sizeof(km_block_t));
}

void* __allocate_block(km_block_t* blk) {
    blk->free = false;
    return (void*)((uint8_t*)blk + sizeof(km_block_t));
}

void init_kmm(void) {
    km_block_t* head = (km_block_t*)mm_mmap(&kheap_zone, INITIAL_KHEAP_PAGES);
    __set_block(head, INITIAL_KHEAP_PAGES * PAGE_SIZE, true, NULL);

    kheap = (uint32_t)head;
}

void* kmalloc(const uint32_t size) {
    km_block_t* cur = (km_block_t*)kheap;

    // find first fitting block.
    while (cur && (cur->size < size || !cur->free)) {
        cur = cur->next;
    }

    if (NULL == cur) {
        return NULL;
    }

    // ensure there's enough space to allocate a header too.
    if (cur->size > (size + sizeof(km_block_t))) {
        return __split_block(cur, size);
    }

    return __allocate_block(cur);
}

void kfree(const void* address) {
    km_block_t* allocblk = (km_block_t*)((uint8_t*)address - sizeof(km_block_t));
    allocblk->free = true;
}
