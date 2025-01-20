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