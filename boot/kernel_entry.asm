[bits 32]
[extern kmain]
; calls the C function. 
; the linker will resolve it.
global _start ; export the symbol so ld can find it.
_start:
    call kmain
    jmp $