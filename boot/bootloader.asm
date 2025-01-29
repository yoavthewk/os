; we're loaded at 0000:7c00 or 07c0:0000
; therefore we use org to set this as 
; our base address.
[org 0x7c00]

; set our kernel to 0x100000 address.
; note the value here is 0xFFFF, but it is shifted by four
; due to the fact we use it in ES:BX. Afterwards, we add bx to it, 
; which is 0x10, and get 0x100000:
; 0xFFFF0 + 0x10 = 0x100000
KERNEL_SEGMENT_VALUE equ 0xFFFF
KERNEL_START equ 0x100000

start:
    mov [BOOT_DRIVE], dl ; save the boot drive if need be.

    mov sp, 0x9c00 ; set our stack to a convenient address (enough space to push).

    ; set up segment registers.
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov si, REAL_MODE_BOOT
    call print

    ; read the kernel from the disk using int 0x13.
    call load_kernel

; If we somehow fail and reach here, hang.
hang:
    jmp $

%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/print.asm"

[bits 16]
load_kernel:
    mov si, LOADING_KERNEL
    call print

    push es ; save the last value.
    ; This is a simple trick to really load our kernel at
    ; 0x100000 using segmentation.
    mov cx, KERNEL_SEGMENT_VALUE
    mov es, cx
    mov bx, 0x10
    ; the number of sectors to read.
    ; each sector is 512 bytes.
    ; 20h * 512 = 16KB of Kernel Code.
    ; might need to increment later on.
    mov dh, 0x20
    mov dl, [BOOT_DRIVE] ; if it somehow gets overriden.
    call disk_read
    pop es ; retrieve the last value. 

    ; print end message.
    mov si, LOADED_KERNEL
    call print

    call pm_switch ; switch to PM and eventually jump to init_kernel

    ret

pm_switch:
    ; disable interrupts.
    cli

    ; load GDT
    lgdt [gdt_descriptor]
    
    ; enable PM in cr0
    mov eax, cr0
    or eax, 1
    mov cr0, eax ; enable PM in cr0.

    ; flush CPU pipeline by making FAR jump.
    jmp CODE_SEG:pm_init


[bits 32]
pm_init:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x90000

    call pm_begin

pm_begin:
    ; print that we switched.
    mov si, PROTECTED_MODE_BOOT
    call pm_print

    ; call the kernel. 
    ; if we are ever returned the control, hang.
    call KERNEL_START
    jmp hang

BOOT_DRIVE db 0
REAL_MODE_BOOT db 'Booted in 16-bit Real Mode...', 0
LOADING_KERNEL db 'Loading kernel from disk @ 0x10000...', 0
LOADED_KERNEL db 'Kernel Loaded...', 0
PROTECTED_MODE_BOOT db 'Switched to 32-bit Protected Mode...', 0
SWITCHING_MODE db 'Switching to Protected Mode...', 0

times 510-($-$$) db 0
dw 0xaa55