#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <kernel/vga.h>

#define V2P(addr) (((uint8_t*)addr - (uint8_t*)VIRT))

extern _start, kernel_start, kernel_end;
page_directory_t* pd;
page_directory_t* kpgdir = (page_directory_t*)PD_ADDRESS;

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

void __recursive_pgdt(void) {
    pd->page_tables[1023] = GET_FRAME((uint32_t)pd) | PDE_RW | PDE_PRESENT; 
}

bool __pt_present(uint32_t virt) {
    return kpgdir->page_tables[PDINDEX(virt)] & PDE_PRESENT;
}

page_table_t* __get_pt(uint32_t virt) {
    return (page_table_t*)(PT_ADDRESS + (PDINDEX(virt) * PAGE_SIZE));
}

bool __is_page_mapped(uint32_t virt) {
    return __get_pt(virt)->pages[PTINDEX(virt)] & PTE_PRESENT;
}

void __init_pt(uint32_t virt) {
    pd_entry_t* pde = &kpgdir->page_tables[PDINDEX(virt)];
    page_table_t* pte = (page_table_t*)kalloc_pg(1);
    __set_frame(pde, pte);
    *pde |= PDE_PRESENT | PDE_RW;
}

void* __get_next_available(mm_zone_t* zone, uint32_t pgnum) {
    uint32_t free_pages = 0;
    void* virt_base = NULL;
    page_table_t* pt = NULL;

    for (uint32_t virt = zone->base; virt < zone->limit; virt += PAGE_SIZE) {
        if (!__pt_present(virt)) {
            // TODO: free if not used?
            __init_pt(virt);
        }

        if (__is_page_mapped(virt)) {
            free_pages = 0;
            continue;
        }

        if (0 == free_pages) {
            virt_base = (void*)virt;
        }
        ++free_pages;

        if (pgnum == free_pages) {
            return virt_base;
        }
    }

    return NULL;
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

    // Seeing as we'll now be in paging mode,
    // we need a way to access the physical addresses of our
    // page tables. For that, we'll map PDE[1023] to &PD.
    // refer to [http://www.rohitab.com/discuss/topic/31139-tutorial-paging-memory-mapping-with-a-recursive-page-directory/]
    // for a really good explanation.
    __recursive_pgdt();

    // Switch the page directory - paging is already enabled from boot.
    __set_pgd(pd);
}

int8_t vmm_map(uint32_t frame, uint32_t virt) {
    if (!__ensure_page_alignment(virt) || !__ensure_page_alignment(frame)) {
        return -1;
    }

    page_table_t* pt = __get_pt(virt);

    if (kpgdir->page_tables[PDINDEX(virt)] & PDE_PRESENT) {
        if (pt->pages[PTINDEX(virt)] & PTE_PRESENT) {
            // already mapped.
            return -1;
        }
    }
    else {
        page_table_t* new_pt = kalloc_pg(1);
        kpgdir->page_tables[PDINDEX(virt)] = GET_FRAME((uint32_t)new_pt) | PDE_PRESENT | PDE_RW;
    }

    pt->pages[PTINDEX(virt)] = GET_FRAME(frame) | PTE_PRESENT | PTE_RW;
    return 0;
}

int8_t vmm_unmap(uint32_t virt) {
    if (!__ensure_page_alignment(virt)) {
        return -1;
    }

    if (!(kpgdir->page_tables[PDINDEX(virt)] & PDE_PRESENT)) {
        return -1;
    }   

    page_table_t* pt = __get_pt(virt);
    if (!(pt->pages[PTINDEX(virt)] & PTE_PRESENT)) {
        return -1;
    }

    pt->pages[PTINDEX(virt)] = PTE_RW;

    // check if the page table needs to stay mapped.
    uint32_t free_pgs = 0;
    for (uint32_t free_pgs = 0; (free_pgs < NUMBER_OF_PAGES) && (!pt->pages[free_pgs] & PTE_PRESENT); ++free_pgs);
    if (NUMBER_OF_PAGES == free_pgs) {
        kfree_pg(GET_FRAME(kpgdir->page_tables[PDINDEX(virt)]));
        kpgdir->page_tables[PDINDEX(virt)] = PDE_RW;
    }

    return 0;
}

// FIXME: very kernel focused, doubt this'll hold up in userspace.
//        improve when possible.
void* mm_mmap(mm_zone_t* zone, uint32_t pgnum) {
    void* virt_address = __get_next_available(zone, pgnum);
    if (NULL == virt_address) {
        return NULL;
    }

    void* phys_address = kalloc_pg(pgnum);
    if (NULL == phys_address) {
        return NULL;
    }

    for (uint32_t i = 0, frame = (uint32_t)phys_address, virt = (uint32_t)virt_address; 
            i < pgnum; ++i, frame += PAGE_SIZE, virt += PAGE_SIZE) {
        if (0 != vmm_map(frame, virt)) {
            return NULL;
        }
    }

    return virt_address;
}