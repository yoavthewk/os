BUILD=build
BOOT=boot

run: $(BUILD)/os-image.bin
	mkdir -p $(BUILD)
	qemu-system-i386 -fda $<

$(BUILD)/os-image.bin: $(BUILD)/bootloader.bin
	cat $^ > $@

$(BUILD)/%.bin: $(BOOT)/%.asm
	nasm $< -f bin -o $@
