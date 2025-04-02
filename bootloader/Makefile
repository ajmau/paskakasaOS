DISK_FILE = ../disk.img
BUILD_DIR = build
BIN_DIR = bin

CFLAGS64 = -m64 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c
CFLAGS32 = -m32 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c

STAGE1OBJS = $(BUILD_DIR)/setup_pmode.o \
			 $(BUILD_DIR)/setup_longmode.o \
			 $(BUILD_DIR)/switch64.o

STAGE2OBJS = $(BUILD_DIR)/stage2.o

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

bootsector: | $(BIN_DIR)
	nasm -f bin bootsector.asm -o $(BIN_DIR)/bootsector.bin

$(BUILD_DIR)/%.o: %.asm | $(BUILD_DIR)
	nasm -f elf32 -o $@ $<

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	clang $(CFLAGS) -o $@ $<

stage1: CFLAGS=$(CFLAGS32)
stage1: $(STAGE1OBJS)
	ld -m elf_i386 -T stage1.ld -o $(BIN_DIR)/$@.elf $(STAGE1OBJS)
	objcopy -O binary $(BIN_DIR)/stage1.elf $(BIN_DIR)/$@.bin

stage2: CFLAGS=$(CFLAGS64)
stage2: $(STAGE2OBJS)
	ld -m elf_x86_64 -T stage2.ld -o $(BIN_DIR)/$@.elf $(STAGE2OBJS)
	objcopy -O binary $(BIN_DIR)/stage2.elf $(BIN_DIR)/$@.bin

bootloader: bootsector stage1 stage2

install_bootloader: $(BIN_DIR)/bootsector.bin
	dd if=$(BIN_DIR)/bootsector.bin of=$(DISK_FILE) conv=notrunc bs=446 count=1

install_stage1: $(BIN_DIR)/stage1.bin
	dd if=$(BIN_DIR)/stage1.bin of=$(DISK_FILE) conv=notrunc bs=512 seek=1

install_stage2: $(BIN_DIR)/stage2.bin
	dd if=$(BIN_DIR)/stage2.bin of=$(DISK_FILE) conv=notrunc bs=512 seek=3

install: install_bootloader install_stage1 install_stage2


clean:
	rm -rf build bin
run:
	qemu-system-x86_64 $(DISK_FILE) -serial file:serial.log

debug:
	gdb -ex "target remote localhost:1234" -ex "symbol-file $(BIN_DIR)/stage2.elf"
