#include <kernel/io/timer/timer.h>
#include <arch/x86/8259/pic.h>
#include <arch/x86/cpu/idt.h>
#include <kernel/vga.h>

uint32_t tticks = 0;

/* Configures the PIT timer to the required HZ. */
void timer_phase(const uint32_t hz) {
    uint32_t divisor = PIT_DIVISOR / hz;
    outb(PIT_CMD, PIT_CONFIGURATION);
    outb(PIT_DATA_0, (divisor & 0xFF));
    outb(PIT_DATA_0, ((divisor >> 8) & 0xFF));
}

void __handle_tick(void) {
    ++tticks;
    kprintfln("tick: %d", tticks);
    // schedule here.
}

void init_pit(void) {
    timer_phase(PIT_HZ);
    idt_install_irq_handler(PIT_IDT_VECTOR, __handle_tick);
    irq_clear_mask(PIT_IRQ_VECTOR);
}