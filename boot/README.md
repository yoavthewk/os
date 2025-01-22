# Writing the Bootloader

While writing this I hadn't seen many information that was missing for me.
This OS is my learning experience; therefore this'll be my learning documentation.

When writing the bootloader there are a few things you need to achieve:

1. set up stack.
2. set segment registers.
3. read kernel from disk and load it.
4. switch to protected mode (or, any other mode).

### Setting up the stack
This step is pretty easy.
We are loaded at 0x7c00, and we want to have a convenient enough space in which we'll use the stack.
For that, we set the stack to be at 0x9c00:
```
mov sp, 0x9c00 ; set our stack to a convenient address (enough space to push).
```
That leaves us 2000h to use it.

### Setting Segment Registers
This is done in two phases; firstly, in 16-bit real mode, and secondly in 32-bit protected mode.

In 16-bit real mode, we do this in order to ensure that we're not relying on our emulator / hardware to set the segment registers to their correct values. 

This is because their values are used implicitly and this can cause some annoying errors.

Therefore, I started off my bootloader with nulling them out:
```
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
```

We'll get into the protected mode initialization later on.

### Read Kernel and Load It

Now, at least in my case, at that point the kernel is stored somewhere on my bootdrive, but it is not loaded yet due to the BIOS only loading 512 bytes of the given image. Therefore, we will need to load it ourselves.

Luckily, BIOS has a nice feature for it. Refering to https://stanislavs.org/helppc/int_13.html is an easy way to understand the usage. I recommend using OSDev for that as well.

We know we're loaded to the 1st sector of the disk. Therefore, if our binary is bigger than 512 bytes; the rest should be in the 2nd sector onwards.

Using the knowledge we now have, writing a procedure to read from the disk is pretty easy:

```
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
```

Then, to load the kernel, we need to decide on the starting offset for it and read the code to that address:
```
[bits 16]
load_kernel:
    mov si, LOADING_KERNEL
    call print

    mov bx, KERNEL_START
    ; the number of sectors to read.
    ; each sector is 512 bytes.
    ; 0x20 * 512 = 16KB of Kernel Code.
    ; might need to increment later on.
    mov dh, 0x20 
    mov dl, [BOOT_DRIVE] ; if it somehow gets overriden.
    call disk_read

    ; print end message.
    mov si, LOADED_KERNEL
    call print

    mov si, KERNEL_START
    call print

    call pm_switch ; switch to PM and eventually jump to init_kernel

    ret
```

### Switching to Protected Mode

To read about protected mode refer to OSDev. 
In order to switch to protected mode we must set up a few things.

1. cli - disable interrupts.
2. load a GDT - this is needed, but it is obsolete so it does not really matter as we will use paging for protection later on. If you're unsure what this means, read up on Segmentation and GDT in OSDev.
3. enable PM in cr0 (bit 0).
4. flush the CPU pipeline by performing a far jump. This means the CPU executes all of the pipelined instructions due to the far jump being unpredictable for it to schedule - this is a partially accurate description of it - for further reading refer to online sources. This also sets the CS to the code segment in the GDT.
5. set the segment registers needed to the new data segment. 
6. give control to the kernel.


