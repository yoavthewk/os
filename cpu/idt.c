#include <common.h>
#include <arch/x86/cpu/idt.h>

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idtr_t idtr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];

    descriptor->isr_low        = LOWER_16(isr);
    descriptor->kernel_cs      = 0x08; // CODE_SEG in GDT.
    descriptor->attributes     = flags;
    descriptor->isr_high       = HIGHER_16(isr);
    descriptor->reserved       = 0;
}

void idt_assemble(void) {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // re-enable interrupts.
}