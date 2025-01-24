#include <stdint.h>
#include <stdbool.h>

typedef struct {
	uint16_t    isr_low;      // the lower 16 bits of the ISR's address
	uint16_t    kernel_cs;    // the GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // null
	uint8_t     attributes;   // type and attributes - refer to IDT in OSDev wiki 
	uint16_t    isr_high;     // the higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

/* IDT register that points to the table. */
typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;

#define IDT_MAX_DESCRIPTORS 256

static bool vectors[IDT_MAX_DESCRIPTORS];
extern void* isr_stub_table[];

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);

void idt_assemble(void);