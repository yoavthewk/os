OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    kernel_start = .;
    .text BLOCK(4K) : ALIGN(4K) {
        code = .;
        *(.text, .text.*)
        . = ALIGN(4096);
    }
    .rodata BLOCK(4K) : ALIGN(4K)
	{
        rodata = .;
		*(.rodata, .rodata.*)
        . = ALIGN(4096);
	}
    .data BLOCK(4K) : ALIGN(4K)
    {
        data = .;
        *(.data, .data.*)
        . = ALIGN(4096);
    }
    .bss BLOCK(4K) : ALIGN(4K)
    {
        bss = .;
        *(.bss, .bss.*)
        . = ALIGN(4096);
    }
    kernel_end = .;
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }

    LMA_data = LOADADDR(.data); 
}
