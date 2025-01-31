#include <kernel/memory/paging.h>
#include <kernel/memory/pmm.h>
#include <kernel/vga.h>

#define V2P(addr) (((uint8_t*)addr - (uint8_t*)VIRT))

extern _start, kernel_start, kernel_end;
page_directory_t* pd;

void __enable_paging(void) {
    __asm__ volatile ("movl %cr0, %eax; orl 0x80000001, %eax; movl %eax, %cr0");
}

void __set_pgd(page_directory_t* pd) {
    __asm__ volatile ("movl %%edx, %%cr3" :: "d"(pd));
}

bool __ensure_page_alignment(uint32_t addr) {
    return 0 == addr % PAGE_SIZE;
}

bool __set_frame(pd_entry_t* entry, uint32_t addr) {
    if (!__ensure_page_alignment(addr)) {
        return false;
    }

    *entry = (*entry & ~0xFFFFF000) | addr;
    return true;
}

void __init_pd(page_directory_t* pd) {
    for (uint32_t index = 0; index < NUMBER_OF_PAGE_TABLES; ++index) {
        pd->page_tables[index] = PDE_RW; // Supervisor, r/w, not present.
    }
}

bool __map_page(page_directory_t* pd, uint32_t frame, uint32_t virt, uint32_t attrs) {
    // Ensure frame/virt are aligned.
    if (!__ensure_page_alignment(frame) || !__ensure_page_alignment(virt)) {
        return false;
    }

    page_table_t* pte;
    pd_entry_t* pde = &pd->page_tables[PDINDEX(virt)];
    if (*pde & PDE_PRESENT) {
        page_table_t* pte = (page_table_t*)GET_FRAME(*pde);
    } 
    else {
        // allocate a page table.
        pte = (page_table_t*)kalloc_pg(1);
        __set_frame(pde, pte);
    }

    *pde |= PDE_PRESENT | PDE_RW;

    pte->pages[PTINDEX(virt)] = frame | (attrs & 0xFFF) | PTE_PRESENT; 

    return true;
}

void __print_paging_done(void) {
    kprint("Done initializing paging! :)\n");
    while(1);
}

void __map_kernel(void) {
    uint32_t kernel_size = get_closest_page((uint8_t*)&kernel_end - (uint8_t*)&kernel_start);
    uint32_t kernel_pages = (kernel_size + MB) / PAGE_SIZE;

    for (uint32_t i = 0, frame = KERNEL_START, virt = VIRT_START; i < kernel_pages; 
                ++i, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        __map_page(pd, frame, virt, PTE_PRESENT | PTE_RW);
    }
}

void __id_map(void) {
    for (uint32_t i = 0, frame = 0, virt = 0; i < NUMBER_OF_PAGES; ++i ,frame += PAGE_SIZE, virt += PAGE_SIZE) {
        __map_page(pd, frame, virt, PTE_PRESENT | PTE_RW);
    }
}

void init_vmm(void) {
    pd = (page_directory_t*)kalloc_pg(1);

    // Initiate page directory to not present.
    __init_pd(pd);

    // Next, we'll need to map our first page table.
    // We'll want to identity map our first 4MiB of memory (a part of the kernel 
    // so the CPU can resolve the addresses).
    __id_map();

    // Now, we'll want to create a map for our kernel.
    // This is a higher half kernel, so let's map it to 3GB+ addresses.
    __map_kernel();

    // Enable paging and set the page directory.
    __set_pgd(pd);
    //__enable_paging();

    // The kernel should now be mapped to the 3GB+ addresses. 
    // Therefore, we'll need to jump there.
    __print_paging_done();
}

