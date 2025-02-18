#include <kernel/process/process.h>
#include <kernel/process/scheduler.h>
#include <kernel/memory/kmm.h>
#include <kernel/memory/vmm.h>
#include <libc/mem.h>
#include <libc/string.h>


proc_t* current_proc = NULL;
proc_t* proc_list = NULL; 

static inline void* __alloc_proc_stack(void) {
    void* stack_bot = mm_mmap(&us_mm, INITIAL_STACK_PAGES);
    return (void*)((uint8_t*)stack_bot + INITIAL_STACK_PAGES * PAGE_SIZE);
}

static inline proc_t* __alloc_proc(proc_t* parent, char* name) {
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
    proc->pd = vmm_create();

    page_directory_t* cur_pgdir = get_pgd();

    // set stack alongside flags etc.
    // we need to set the pgdir to the new vm in order
    // to ensure we're allocating stack in our mm zone.
    // We later restore the state.
    set_pgd(proc->pd);
    uint32_t stack = (uint32_t)__alloc_proc_stack();
    set_pgd(cur_pgdir);

    proc->context.useresp = stack;
    proc->context.eflags = ALLOC_PROC_FLAGS;
    proc->context.ebp = 0;

    return proc;
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

proc_t* create_proc(proc_t* parent, char* name, uint32_t entry) {
    proc_t* proc = __alloc_proc(parent, name);
    proc->context.eip = entry;
    add_proc(proc);

    return proc;
}
