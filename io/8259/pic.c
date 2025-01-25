#include <arch/x86/8259/pic.h>
#include <arch/x86/cpu/comm.h>

/* 
 * Remaps the PIC to not override the ISRs.
 * m_offset - vector offset for master PIC.
 * s_offset - vector offset for slave PIC.
 * vectors on master become m_offset..m_offset+7
 * vectors on slave become s_offset..s_offset+7
**/
void pic_remap(uint8_t m_offset, uint8_t s_offset) {
    uint8_t m_mask, s_mask;

    // save masks.
    m_mask = inb(MASTER_PIC_DATA);
    s_mask = inb(SLAVE_PIC_DATA);

    // Start initialization sequence.
    __send_pic_command(MASTER_PIC_CMD, ICW1_INIT | ICW1_ICW4);
    __send_pic_command(SLAVE_PIC_CMD, ICW1_INIT | ICW1_ICW4);

    // ICW2: Configure the new offsets.
    __send_pic_command(MASTER_PIC_DATA, m_offset);
    __send_pic_command(SLAVE_PIC_DATA, s_offset);

    // ICW3: Configure identities.
    // tell master there's a slave PIC at IRQ2.
    __send_pic_command(MASTER_PIC_DATA, 0x0100); 
    // tell slave PIC its cascade identity.
    __send_pic_command(SLAVE_PIC_DATA, 0x0010);

    // use 8086 mode.
    __send_pic_command(MASTER_PIC_DATA, ICW4_8086);
    __send_pic_command(SLAVE_PIC_DATA, ICW4_8086);

    // restore masks.
    outb(MASTER_PIC_DATA, m_mask);
    outb(SLAVE_PIC_DATA, s_mask);
}

void pic_init(void) {
    pic_remap(MASTER_OFFSET, SLAVE_OFFSET);
    
    // Disable all IRQs.
    for (uint8_t irq = 0; irq < IRQ_COUNT; ++irq) {
        irq_set_mask(irq);
    }
}

void pic_send_eoi(uint8_t irq_vector) {
    /* If the IRQ is from the slave, issue EOI to both chips. */
    if (irq_vector >= MASTER_PIC_MAX_IRQ) {
        outb(SLAVE_PIC_CMD, PIC_EOI);
    }
    outb(MASTER_PIC_CMD, PIC_EOI);
}

void pic_disable(void) {
    outb(MASTER_PIC_DATA, PIC_DISABLE);
    outb(SLAVE_PIC_DATA, PIC_DISABLE);
}

void irq_set_mask(uint8_t irq_vector) {
    uint16_t port = __port_from_vector(irq_vector);
    if (port == SLAVE_PIC_DATA) {
        irq_vector -= 8;
    }
    uint8_t mask = inb(port) | (1 << irq_vector); 
    outb(port, mask);
}

void irq_clear_mask(uint8_t irq_vector) {
    uint16_t port = __port_from_vector(irq_vector);
    if (port == SLAVE_PIC_DATA) {
        irq_vector -= 8;
    }
    uint8_t mask = inb(port) & ~(1 << irq_vector);
    outb(port, mask);
}

void __send_pic_command(uint16_t port, uint8_t val) {
    outb(port, val);
    io_wait();
}

uint16_t __port_from_vector(uint8_t irq_vector) {
    return irq_vector < MASTER_PIC_MAX_IRQ ? MASTER_PIC_DATA : SLAVE_PIC_DATA;
}