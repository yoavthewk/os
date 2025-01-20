disk_read:
    push dx
    mov ah, 0x02 ; set to read.
    mov al, dh ; set number of sectors to read.
    mov ch, 0
    mov cl, 0x02 ; sectors start from one. sector 1 is the bootloader.
    mov dh, 0

    int 0x13
    jc disk_read_error

    mov si, DONE_READING
    call print

    ; restore the number of sectors read.
    pop dx
    cmp dh, al ; al has number of sectors read.
    jne sector_read_error

    ret

disk_read_error:
    mov si, DISK_READ_ERROR
    call print
    jmp hang

sector_read_error:
    mov si, SECTOR_READ_ERROR
    call print
    jmp hang

disk_hang:
    jmp $

DISK_READ_ERROR db 'Error occurred during disk read...', 0
SECTOR_READ_ERROR db 'Incorrect number of sectors read...', 0
DONE_READING db 'Done reading disk...', 0


