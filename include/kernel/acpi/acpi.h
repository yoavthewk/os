#pragma once
#include <stdint.h>

/* --- RSDP --- */
#define RSDP_REG_START 0xE0000
#define RSDP_REG_END 0xFFFFF
#define RSDP_SIG_LEN 8

typedef struct {
 char sig[8];
 uint8_t csum;
 char oemid[6];
 uint8_t revision;
 uint32_t rsdt_ptr;
} __attribute__ ((packed)) rsdp_t;

typedef struct {
  char sig[4];
  uint32_t len;
  uint8_t revision;
  uint8_t csum;
  char oemid[6];
  char oem_tableid[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__ ((packed)) acpisdt_header_t;

typedef struct {
    acpisdt_header_t header;
    uint32_t entries[];
} __attribute__ ((packed)) rsdt_t;

typedef struct
{
    uint8_t address_space_id; // 0 - system memory, 1 - system I/O
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} __attribute__((packed)) address_t;

static const uint8_t RSDP_SIG[] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};

void parse_acpi(void);
