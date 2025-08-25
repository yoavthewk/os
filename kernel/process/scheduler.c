#include <kernel/memory/kmm.h>
#include <kernel/process/scheduler.h>
#include <kernel/process/process.h>
#include <kernel/vga.h>

void __idle(void) {
    kprintln("In first process!");
    while(true);
}

void __idle2(void) {
    kprintln("In second process!");
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
                kprintfln("Removing process %s", preemp_proc->name);
                remove_proc(preemp_proc);
                break;
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
    // do not context switch when it is the same process.
    // this introduces a heavy overload.
    if (proc == current_proc) {
        return;
    }

    // this might be a takeoff from kernel,
    // in which case we do not want to preserve stack.
    if (RUNNING == current_proc->status){
        current_proc->status = READY;
        current_proc->context = *frame; 
    }

    proc->status = RUNNING;
    *frame = proc->context;
    
    // switch pgdir.
    set_pgd(proc->mem->pgdir);
}

uint32_t scheduler_gen_pid(void) {
    static uint32_t pid = 1;

    return pid++;
}

void schedule(irq_frame_t* frame) {
    proc_t* next_proc = __schedule_rr();
    __context_switch(next_proc, frame);
    current_proc = next_proc;
}

void init_scheduler(void) {
    // create the idle proc.
    proc_list = create_proc(NULL, "init", (uint32_t)__idle);
    create_proc(proc_list, "joebiden", (uint32_t)__idle2);
}