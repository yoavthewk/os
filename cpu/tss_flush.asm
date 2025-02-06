global tss_flush

section .text

tss_flush:
    mov ax, 0x28 ; the TSS segment selector.
    ltr ax
    ret