BUILD=build
BOOT=boot
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
OBJ = ${C_SOURCES:.c=.o}

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /bin/gdb
CFLAGS = -g -m32 -ffreestanding

run: $(BUILD)/os.bin
	mkdir -p $(BUILD)
	qemu-system-i386 -fda $<

$(BUILD)/os.bin: $(BUILD)/bootloader.bin $(BUILD)/kernel.bin
	cat $^ > $(BUILD)/os.bin

debug: $(BUILD)/os.bin $(BUILD)/kernel.elf
	qemu-system-i386 -s -fda $< -d guest_errors,int &
	${GDB} -ex "symbol-file $(BUILD)/kernel.elf" -e "b kmain" -ex "target remote localhost:1234"

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	objcopy -O binary $< $@

$(BUILD)/kernel.elf: $(BUILD)/kernel_entry.o $(OBJ)
	$(CC) -ffreestanding -nostdlib -g -Tlinker.ld $^ -o $@ -lgcc

$(BUILD)/%.bin: $(BOOT)/%.asm
	nasm $< -f bin -o $@

$(BUILD)/%.o: $(BOOT)/%.asm
	nasm $< -f elf -o $@

$(BUILD)/%.o: $(C_DIRS)/%.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm */*.o
