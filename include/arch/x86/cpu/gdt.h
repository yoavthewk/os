#pragma once
#include <stdint.h>

#define GDT_ENTRIES 6
#define DEFAULT_FLAGS 0xCF
#define X86_MAX 0xFFFFFFFF

enum gdt_bits_t {
    /// @brief `0b10000000U` (Present): This must be 1 for all valid selectors.
    GDT_PRESENT = 128U,
    /// @brief `0b00000000U` (Privilege): Sets the 2 privilege bits (ring level) to 0 = highest (kernel).
    GDT_KERNEL = 0U,
    /// @brief `0b01100000U` (Privilege): Sets the 2 privilege bits (ring level) to 3 = lowest (user applications).
    GDT_USER = 96U,
    /// @brief `0b00010000U` (Descriptor): This bit should be set for code or data segments and should be cleared for system segments (eg. a Task State Segment)
    GDT_S = 16U,
    /// @brief `0b00001000U` (Executable): If 1 code in this segment can be executed, ie. a code selector. If 0 it is a data selector.
    GDT_EX = 8U,
    /// @brief `0b00000100U` (Direction/Conforming)
    GDT_DC = 4U,
    /// @brief `0b00000010U` (Readable/Writable)
    GDT_RW = 2U,
    /// @brief `0b00000001U` (Accessed): Just set to 0. The CPU sets this to 1 when the segment is accessed.
    GDT_AC = 1U,
    /// @brief `0b00001100U` (Executable Code): Identifies an executable code segment.
    GDT_CODE = (GDT_S | GDT_EX),
    /// @brief `0b00001001U` (Writable Data): Identifies a writable data segment.
    GDT_DATA = (GDT_S | GDT_RW),
    /// @brief `0b10000000U` (Granularity): If 0 the limit is in 1 B blocks (byte granularity), if 1 the limit is in 4 KiB blocks (page granularity).
    GDT_GRANULARITY = 128U,
    /// @brief `0b01000000U` (Size): If 0 the selector defines 16 bit protected mode. If 1 it defines 32 bit protected mode. You can have both 16 bit and 32 bit selectors at once.
    GDT_OPERAND_SIZE = 64U,
};

typedef struct {
    uint16_t limit_low; 
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    // Limit_Hi(4) AVL L D/B G
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t address;
} __attribute__((packed)) gdt_reg_t;

void init_gdt(void);

void set_gdt_entry(
    const uint8_t index, const uint32_t base, 
    const uint32_t limit, const uint8_t access,
    const uint8_t granularity
);
