#pragma once
#include <kernel/memory/vmm.h>
#include <kernel/memory/pmm.h>
#include <arch/x86/cpu/int.h>

#define TASK_NAME_MAX_LEN 100
#define INITIAL_STACK_PAGES 0x4
#define USERSPACE_START 0x400000
#define KERNEL_START 0xC0000000
#define KERNEL_SS 0x10
#define KERNEL_CS 0x08
#define ALLOC_PROC_FLAGS 0x202

typedef enum {
    READY,
    RUNNING,
    SLEEPING,
    DEAD,
} proc_status_t;

typedef struct {
    proc_status_t status;
    uint32_t pid;
    procmem_t* mem;
    irq_frame_t context;
    struct proc_t* parent;
    struct proc_t* next;
    char name[TASK_NAME_MAX_LEN];
    uint32_t stack;
} proc_t;

static mm_zone_t us_mm = {
    .base = USERSPACE_START,
    .limit = KERNEL_START,
};

extern proc_t* current_proc;
extern proc_t* proc_list; 

proc_t* create_proc(proc_t* parent, char* name, uint32_t entry);
void add_proc(proc_t* proc);
proc_t* __get_back_proc(proc_t* proc);
void remove_proc(proc_t* proc);

