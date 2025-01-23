BUILD=build
BOOT=boot
C_SOURCES = $(wildcard kernel/*.c cpu/*.c drivers/*.c libc/*/*.c)
HEADERS = $(wildcard kernel/*.h cpu/*.h drivers/*.h libc/*/*.h)
OBJ = $(patsubst %.c, $(BUILD)/%.o, $(notdir $(C_SOURCES)))

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /bin/gdb
CFLAGS = -g -m32 -ffreestanding

# So make knows where to look for .c files.
vpath %.c kernel drivers cpu libc libc/string libc/memory

run: $(BUILD)/os.bin
	mkdir -p $(BUILD)
	qemu-system-i386 -fda $<

$(BUILD)/os.bin: $(BUILD)/bootloader.bin $(BUILD)/kernel.bin
	cat $^ > $(BUILD)/os.bin

debug: $(BUILD)/os.bin $(BUILD)/kernel.elf
	qemu-system-i386 -s -fda $< -d guest_errors,int &
	${GDB} -ex "symbol-file $(BUILD)/kernel.elf" -ex "target remote localhost:1234" -ex "b itoa"

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	objcopy -O binary $< $@

$(BUILD)/kernel.elf: $(BUILD)/kernel_entry.o $(OBJ)
	$(CC) -ffreestanding -nostdlib -g -Tlinker.ld $^ -o $@ -lgcc

$(BUILD)/%.bin: $(BOOT)/%.asm
	nasm $< -f bin -o $@

$(BUILD)/%.o: $(BOOT)/%.asm
	nasm $< -f elf -o $@

$(BUILD)/%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm */*.o
