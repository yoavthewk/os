BUILD=build
CPU=cpu
BOOT=boot
C_SOURCES = $(wildcard kernel/*.c kernel/*/*.c cpu/*.c drivers/*.c libc/*/*.c io/*/*.c io/*/*/*.c)
ASM_SOURCES = $(wildcard cpu/*.asm)
HEADERS = $(wildcard kernel/*.h cpu/*.h drivers/*.h libc/*/*.h)
OBJ = $(patsubst %.c, $(BUILD)/%.o, $(notdir $(C_SOURCES))) \
      $(patsubst %.asm, $(BUILD)/%.o, $(notdir $(ASM_SOURCES)))

CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /bin/gdb
CFLAGS = -g -m32 -ffreestanding -Iinclude

# So make knows where to look for .c files.
vpath %.c kernel kernel/memory drivers cpu libc libc/string libc/memory io/8259 io/hid/ps2
vpath %.asm cpu

run: $(BUILD)/os.bin
	mkdir -p $(BUILD)
	qemu-system-i386 -fda $< -d guest_errors,int -no-reboot

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

$(BUILD)/%.o: $(CPU)/%.asm
	nasm $< -f elf -o $@

$(BUILD)/%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm */*.o build/*.bin
