#include <common.h>
#include <arch/x86/cpu/idt.h>
#include <arch/x86/cpu/isr.h>

__attribute__((aligned(0x10)))
static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static idtr_t idtr;

irq_handler_t __routine_handlers[IDT_MAX_DESCRIPTORS] = { 0 };

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

    for (uint8_t vector = 0; vector < 15; vector++) {
        idt_set_descriptor(vector + 32, irq_stub_table[vector], 0x8E);
        vectors[vector + 32] = true;
    }

    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // re-enable interrupts.
}

void idt_install_irq_handler(uint8_t int_no, irq_handler_t handler) {
    __routine_handlers[int_no] = handler;
}

void irq_handler(irq_frame_t* frame) {
    if (__routine_handlers[(uint8_t)frame->int_no]){
        __routine_handlers[(uint8_t)frame->int_no](frame);
    }
    pic_send_eoi((uint8_t)frame->irq_vector);
}
