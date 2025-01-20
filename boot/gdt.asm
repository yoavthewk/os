; This defines the GDT table.
; refer to https://wiki.osdev.org/GDT_Tutorial
; for information regarding this table and similar data structures.
; db - 1 byte
; dw - 2 bytes
; dd - 4 bytes

gdt_null: 
    ; the GDT starts with a null descriptor -> 8 null bytes.
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; GDT for code segment. base = 0x00000000, length = 0xfffff
; for flags, refer to https://wiki.osdev.org/Global_Descriptor_Table
gdt_code: 
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; GDT for data segment. base and length identical to code segment
; cleared executable bit in access byte flags.
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_null - 1 ; u32 (16 bit), always one less of its true size
    dd gdt_null ; address (32 bit)

; define some constants for later use
CODE_SEG equ gdt_code - gdt_null
DATA_SEG equ gdt_data - gdt_null