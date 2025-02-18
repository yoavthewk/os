#include <kernel/memory/kmm.h>
#include <kernel/process/scheduler.h>
#include <kernel/process/process.h>

void __idle(void) {
    while (true) {
        asm ("hlt");
    }
}

proc_t* __get_back_proc(proc_t* proc) {
    proc_t* cur_proc = proc->next;
    while (cur_proc->next != proc) {
        cur_proc = NULL == proc->next ? proc_list : proc->next;
    }
    return cur_proc;
}

void __remove_proc(proc_t* proc) {
    proc_t* back_proc = __get_back_proc(proc);
    back_proc->next = proc->next;
    kfree(proc);
}

proc_t* __schedule_rr() { 
    // round-robin - go in a circular manner
    // and find the first ready process.
    proc_t* preemp_proc = current_proc->next;

    while (preemp_proc != current_proc) {
        if (NULL == preemp_proc) {
            preemp_proc = proc_list;
        }

        switch (preemp_proc->status) {
            case DEAD:
                __remove_proc(preemp_proc);
            case READY:
                return preemp_proc;
            default:
                continue;
        }
        if (NULL == preemp_proc->next) {
            preemp_proc = proc_list;
        }
        else {
            preemp_proc = preemp_proc->next;
        }
    }

    // there's no other process, return the current one.
    // there cannot be none, as there's always the idle process.
    return current_proc;
}

void __context_switch(proc_t* proc, irq_frame_t* frame) {
    irq_frame_t context = proc->context;

    current_proc->status = READY;
    current_proc->context = *frame; 

    proc->status = RUNNING;
    *frame = context;
    
    // switch pgdir.
    set_pgd(proc->pd);
}

uint32_t scheduler_gen_pid(void) {
    static uint32_t pid = 1;

    return pid++;
}

void schedule(irq_frame_t* frame) {
    proc_t* next_proc = __schedule_rr();
    __context_switch(next_proc, frame);
}

void init_scheduler(void) {
    // create the idle proc.
    proc_list = create_proc(NULL, "init", (uint32_t)__idle);
}