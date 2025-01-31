[bits 32]
[extern kmain]

; Need to enable paging here in order to jump to the kernel in the
; correct addresses.
section .bs_stack, "aw", @nobits
stack_bottom:
    resb 4096 ; 16KiB
stack_top:

section .boot_bss, "aw", @nobits
    align 4096
boot_page_directory:
    resb 4096
boot_page_table:
    resb 4096

section .lower.text
global _start ; export symbol so ld finds it.
_start:
    lea edi, [boot_page_table - 0xC0000000]
    mov esi, 0
    mov ecx, 1024

map_pages:
    cmp ecx, 0
    je done_mapping

    mov edx, esi
    or edx, 0x003
    mov [edi], edx    

next_page_entry:
    add esi, 4096 ; next frame.
    add edi, 4 ; next page entry.
    loop map_pages

    jmp done_mapping

    call kmain
    jmp $

done_mapping:
    mov dword [boot_page_directory - 0xC0000000], boot_page_table - 0xC0000000 + 0x03
    mov dword [boot_page_directory - 0xC0000000 + 768 * 4], boot_page_table - 0xC0000000 + 0x03

    lea ecx, [boot_page_directory - 0xC0000000]
    mov cr3, ecx

    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    lea ecx, [higher_half]
    jmp ecx

section .text
higher_half:
    ; mov dword [boot_page_directory + 0], 0
    mov ecx, cr3
    mov cr3, ecx
    mov esp, stack_top

    call kmain

    ; If control is somehow returned.
    jmp $
