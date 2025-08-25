#include <kernel/process/process.h>
#include <kernel/process/scheduler.h>
#include <kernel/memory/kmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/common.h>
#include <kernel/vga.h>
#include <libc/mem.h>
#include <libc/string.h>


proc_t* current_proc = NULL;
proc_t* proc_list = NULL; 

static inline void* __alloc_proc_stack(void) {
    void* stack_bot = mm_mmap(&us_mm, INITIAL_STACK_PAGES);
    return (void*)((uint8_t*)stack_bot + (INITIAL_STACK_PAGES - 1) * PAGE_SIZE);
}

static inline proc_t* __alloc_proc(proc_t* parent, char* name, uint32_t entry) {
    proc_t* proc = kmalloc(sizeof(proc_t));
    memset(proc, 0, sizeof(proc_t));

    proc->pid = scheduler_gen_pid();
    strcpy(name, proc->name);
    proc->status = READY;
    proc->parent = parent;

    // set segments.
    proc->context.ss = KERNEL_SS;
    proc->context.cs = KERNEL_CS;

    // create a pagedir and change to it.
    proc->mem = vm_create();

    page_directory_t* cur_pgdir = get_pgd();

    // set stack alongside flags etc.
    // we need to set the pgdir to the new vm in order
    // to ensure we're allocating stack in our mm zone.
    // We later restore the state.
    set_pgd(proc->mem->pgdir);
    proc->stack = (uint32_t)__alloc_proc_stack();

    // pass the entry point as an argument.
    *(uint32_t*)(proc->stack) = entry;
    set_pgd(cur_pgdir);

    proc->context.useresp = proc->stack;
    proc->context.eflags = ALLOC_PROC_FLAGS;

    return proc;
}

proc_t* __get_back_proc(proc_t* proc) {
    proc_t* cur_proc = proc->next;
    while (cur_proc->next != proc) {
        cur_proc = NULL == proc->next ? proc_list : proc->next;
    }
    return cur_proc;
}

void remove_proc(proc_t* proc) {
    proc_t* back_proc = __get_back_proc(proc);
    back_proc->next = proc->next;
    
    page_directory_t* cur_pgdir = get_pgd();
    set_pgd(proc->mem->pgdir);
    vmm_unmap(proc->stack);
    set_pgd(cur_pgdir);

    kfree(proc->mem->pgdir);
    kfree(proc->mem);
    kfree(proc);
}

void add_proc(proc_t* proc) {
    if (NULL == current_proc) {
        current_proc = proc;
    }
    else {
        proc_t* cur_proc = current_proc;
        while (NULL != cur_proc->next) {
            cur_proc = cur_proc->next;
        }
        cur_proc->next = proc;
        proc->next = NULL; 
    }
}

void __kproc_exit(void) {
    cli();
    current_proc->status = DEAD;
    kprintfln("Process %s exiting...", current_proc->name);
    sti();
    while (true);
}

void __kproc_wrapper(void (*entry)(void)) {
    __asm__ volatile ("mov %%edi, %%eax" : "=a"(entry));
    entry();
    __kproc_exit();
}

void __proc_yield(void) {
    __asm__ volatile ("int $0x20" ::: "memory");
}

proc_t* create_proc(proc_t* parent, char* name, uint32_t entry) {
    proc_t* proc = __alloc_proc(parent, name, entry);
    proc->context.eip = __kproc_wrapper;
    proc->context.edi = entry;
    add_proc(proc);

    return proc;
}
