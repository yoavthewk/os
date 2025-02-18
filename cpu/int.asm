extern isr_handler
extern irq_handler

; Define all of the isr_stubs.
; note that isr_err_stub pushes the error code first. 
%macro isr_err_stub 1
isr_stub_%+%1:
    push eax
    push dword eax
    push dword %1

    ; save registers.
    pusha

    ; save segment registers.
    push ds
    push es
    push fs
    push gs

    mov eax, esp
    push eax
    call isr_handler
    pop eax ; pop stack

    ; restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; restore registers.
    popa

    add esp, 8
    pop eax ; restore eax
    sti
    iret 
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push eax
    push dword %1 ; keep the stack uniform
    push dword %1

    ; save registers.
    pusha

    ; save segment registers.
    push ds
    push es
    push fs
    push gs

    mov eax, esp
    push eax
    call isr_handler
    pop eax ; pop stack.

    ; restore segment registers
    pop gs
    pop fs
    pop es
    pop ds

    ; restore registers.
    popa

    add esp, 8
    pop eax ; restore eax
    sti
    iret
%endmacro

%macro irq_stub 2
irq_stub_%+%1:
    cli

    push dword %2      ; push irq_vector (will end up at offset 52)
    push dword %1       ; push int_no (offset 48)

    ; save registers.
    pusha

    ; save segment registers.
    push ds
    push es
    push fs
    push gs

    push esp
    call irq_handler
    add esp, 4

    ; restore segment registers
    pop gs
    pop fs
    pop es
    pop ds
    
    ; restore registers.
    popa

    add esp, 8
    sti
    iret
%endmacro

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
irq_stub 32, 0
irq_stub 33, 1
irq_stub 34, 2
irq_stub 35, 3
irq_stub 36, 4
irq_stub 37, 5
irq_stub 38, 6
irq_stub 39, 7
irq_stub 40, 8
irq_stub 41, 9
irq_stub 42, 10
irq_stub 43, 11
irq_stub 44, 12
irq_stub 45, 13
irq_stub 46, 14
irq_stub 47, 15

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dd isr_stub_%+i
%assign i i+1 
%endrep

global irq_stub_table
irq_stub_table:
%assign i 32 
%rep    15 
    dd irq_stub_%+i
%assign i i+1 
%endrep