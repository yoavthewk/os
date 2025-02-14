extern isr_handler
extern irq_handler

; Define all of the isr_stubs.
; note that isr_err_stub pushes the error code first. 
%macro isr_err_stub 1
isr_stub_%+%1:
    push eax
    push byte %1
    mov eax, esp
    push eax
    call isr_handler
    pop eax ; pop stack
    add esp, 8
    pop eax ; restore eax
    sti
    iret 
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push eax
    push byte %1 ; keep the stack uniform.
    push byte %1
    mov eax, esp
    push eax
    call isr_handler
    pop eax ; pop stack.
    add esp, 8
    pop eax ; restore eax
    sti
    iret
%endmacro

%macro irq_stub 1
irq_stub_%+%1:
    push eax
    push ecx
    mov ecx, %1
    sub ecx, 32 ; find the IRQ number.
    push ecx ; keep the stack uniform.
    push byte %1
    mov eax, esp
    push eax
    call irq_handler
    pop ecx 
    pop eax ; pop stack.
    add esp, 8
    pop eax ; restore eax
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
irq_stub 32
irq_stub 33
irq_stub 34
irq_stub 35
irq_stub 36
irq_stub 37
irq_stub 38
irq_stub 39
irq_stub 40
irq_stub 41
irq_stub 42
irq_stub 43
irq_stub 44
irq_stub 45
irq_stub 46
irq_stub 47

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