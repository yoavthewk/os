OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS
{
    . = 0x00100000;
    phys_kernel_start = .;
    .lower.text : {
        *(.multiboot.text)
    }

    . += 0xC0000000;
    kernel_start = .;
    .text ALIGN(4K): AT(ADDR(.text) - 0xC0000000) {
        code = .;
        *(.text, .text.*)
        . = ALIGN(4096);
    }
    .rodata ALIGN(4K): AT(ADDR(.rodata) - 0xC0000000)
	{
        rodata = .;
		*(.rodata, .rodata.*)
        . = ALIGN(4096);
	}
    .data ALIGN(4K): AT(ADDR(.data) - 0xC0000000)
    {
        data = .;
        *(.data, .data.*)
        . = ALIGN(4096);
    }
    .bss ALIGN(4K): AT(ADDR(.bss) - 0xC0000000)
    {
        bss = .;
        *(.bss, .bss.*)
        *(.boot_bss)
        *(.bs_stack)
        . = ALIGN(4096);
    }
    kernel_end = .;
    phys_kernel_end = . - 0xC0000000;
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }

    LMA_data = LOADADDR(.data); 
}
