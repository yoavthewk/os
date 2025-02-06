global gdt_flush

section .text

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax] ; load the new GDT register.
    jmp 0x08:flush

flush:
    ; Set the segment registers to the new data segment.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret