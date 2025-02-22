#pragma once
#include <stdint.h>
#include <stdbool.h>

#define KERNEL_START 0
#define VIRT_START 0xC0000000
#define KERNEL_VIRT_START 0xC0100000
#define KERNEL_VIRT_END 0xFFFFFFFF

#define PAGE_SHIFT 12
// 4KiB.
#define PAGE_SIZE (1 << PAGE_SHIFT)
#define MB_SHIFT 20
#define MB (1 << MB_SHIFT)
#define NUMBER_OF_PAGES 1024
#define NUMBER_OF_PAGE_TABLES 1024

#define PDINDEX(virt) (virt >> 22)
#define PTINDEX(virt) ((virt >> 12) & 0x03FF)
#define GET_FRAME(addr) (addr & 0xFFFFF000)
#define SET_FRAME(entry, addr) (*entry = (*entry & ~0xFFFFF000) | addr)

// Recursive-paging.
#define PD_ADDRESS 0xFFFFF000
#define PT_ADDRESS 0xFFC00000

typedef uint32_t pd_entry_t; 
typedef uint32_t pt_entry_t;

typedef enum {
    PTE_PRESENT       = 0x01,
    PTE_RW            = 0x02,
    PTE_USER          = 0x04,
    PTE_WT            = 0x08,
    PTE_CD            = 0x10,
    PTE_ACCESSED      = 0x20,
    PTE_DIRTY         = 0x40,
    PTE_PAT           = 0x80,
    PTE_GLOBAL        = 0x100,
    PTE_FRAME         = 0x7FFFF000, // bits 12+
} pt_flags_t;

typedef enum {
    PDE_PRESENT       = 0x01,
    PDE_RW            = 0x02,
    PDE_USER          = 0x04,
    PDE_WT            = 0x08,
    PDE_CD            = 0x10,
    PDE_ACCESSED      = 0x20,
    PDE_DIRTY         = 0x40,          // 4MB entry only
    PDE_PAGE_SIZE     = 0x80,          // 0 = 4KB page, 1 = 4MB page
    PDE_GLOBAL        = 0x100,         // 4MB entry only
    PDE_PAT           = 0x2000,        // 4MB entry only
    PDE_FRAME         = 0x7FFFF000,    // bits 12+
} pd_flags_t;

typedef struct {
    pt_entry_t pages[NUMBER_OF_PAGES];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef struct {
    pd_entry_t page_tables[NUMBER_OF_PAGE_TABLES];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

typedef struct {
    uint32_t base;
    uint32_t limit;
} mm_zone_t;

void init_vmm(void);

int8_t vmm_map(uint32_t frame, uint32_t virt);

int8_t vmm_unmap(uint32_t virt);

void* mm_mmap(mm_zone_t* zone, uint32_t pgnum);

void* mm_mmap_phys(mm_zone_t* zone, uint32_t pgnum, void* phys_address);

page_directory_t* vm_create(void);

page_directory_t* get_pgd(void);

void set_pgd(page_directory_t*);
