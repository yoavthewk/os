#pragma once
#include <stdint.h>

#define MASTER_PIC 0x20
#define SLAVE_PIC 0xA0
#define MASTER_PIC_CMD MASTER_PIC
#define MASTER_PIC_DATA (MASTER_PIC + 1)
#define SLAVE_PIC_CMD SLAVE_PIC
#define SLAVE_PIC_DATA (SLAVE_PIC + 1)
#define PIC_EOI 0x20
#define PIC_DISABLE 0xFF
#define MASTER_PIC_MAX_IRQ 0x08

#define MASTER_OFFSET 0x20
#define SLAVE_OFFSET (MASTER_OFFSET + 8)
#define IRQ_COUNT 0x0F

/* Remapping Constants. */
#define ICW1_ICW4	0x01		// Indicates that ICW4 will be present
#define ICW1_SINGLE	0x02		// Single (cascade) mode
#define ICW1_INTERVAL4	0x04	// Call address interval 4 (8)
#define ICW1_LEVEL	0x08		// Level triggered (edge) mode
#define ICW1_INIT	0x10		// Initialization - required! 

#define ICW4_8086	0x01		// 8086/88 (MCS-80/85) mode 
#define ICW4_AUTO	0x02		// Auto (normal) EOI 
#define ICW4_BUF_SLAVE	0x08	// Buffered mode/slave 
#define ICW4_BUF_MASTER	0x0C	// Buffered mode/master
#define ICW4_SFNM	0x10		// Special fully nested (not) 

void pic_remap(uint8_t m_offset, uint8_t s_offset);
void pic_init(void);
void pic_send_eoi(uint8_t irq_vector);
void pic_disable(void);
void irq_set_mask(uint8_t irq_vector);
void irq_clear_mask(uint8_t irq_vector);
void __send_pic_command(uint16_t port, uint8_t val);
uint16_t __port_from_vector(uint8_t irq_vector);