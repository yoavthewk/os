; we're loaded at 0000:7c00 or 07c0:0000
; therefore we use org to set this as 
; our base address.
[org 0x7c00]

; set our kernel to 0x1000 address.
KERNEL_START equ 0x1000

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

    mov bx, KERNEL_START
    ; the number of sectors to read.
    ; each sector is 512 bytes.
    ; 20h * 512 = 16KB of Kernel Code.
    ; might need to increment later on.
    mov dh, 0x20 
    mov dl, [BOOT_DRIVE] ; if it somehow gets overriden.
    call disk_read

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

    ; TODO: enable A20 line?

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
LOADING_KERNEL db 'Loading kernel from disk @ 0x1000...', 0
LOADED_KERNEL db 'Kernel Loaded...', 0
PROTECTED_MODE_BOOT db 'Switched to 32-bit Protected Mode...', 0
SWITCHING_MODE db 'Switching to Protected Mode...', 0

times 510-($-$$) db 0
dw 0xaa55
times 512 db 'Y'
times 512 db 'A'