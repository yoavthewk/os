print:
    push ax
    mov ah, 0x0e

print_loop:
    ; load byte, check if null
    ; and if not print it.
    lodsb
    test al, al
    jz end_print
    int 0x10
    jmp print_loop

end_print:
    ; print newline.
    mov al, 0x0a 
    int 0x10
    mov al, 0x0d
    int 0x10

    ; restore stack to previous state and return.
    pop ax
    ret

[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE equ 0x0f

pm_print:
    push ax

    mov ah, 0x0f 
    mov ebx, VIDEO_MEMORY

pm_print_loop:
    lodsb
    test al, al
    jz end_pm_print
    mov [ebx], ax
    add ebx, 2
    
    jmp pm_print_loop

end_pm_print:
    pop ax
    ret
