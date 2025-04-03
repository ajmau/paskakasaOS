CFLAGS = -I./include -g -m64 -ffreestanding -nostdlib -target x86_64-unknown-none -c

BUILD_DIR = build

SRCS = start.c \
       interrupts/interrupts.c \
       utils/mem.c \
       utils/vesa.c \
       utils/mem.c  \
       utils/portio.c  \
       storage/fat32.c \
       storage/ata_pio.c 

ASM_SRCS = interrupts/interrupt_vectors.asm \
           interrupts/stub.asm

OBJS = $(SRCS:%.c=$(BUILD_DIR)/$(notdir %.o))
ASMOBJS = $(ASM_SRCS:%.asm=$(BUILD_DIR)/$(notdir %.o))

all: kernel

# start.o needs to be first in flat binary for it to be in right address
kernel: $(OBJS) $(ASMOBJS) | $(BUILD_DIR)
	mv build/start.o . 
	ld -m elf_x86_64 -T linker.ld -o boot.elf start.o build/*
	objcopy -O  binary boot.elf boot.bin

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(BUILD_DIR)  
	clang $(CFLAGS) $< -o $(BUILD_DIR)/$(notdir $@)

$(BUILD_DIR)/%.o: %.asm
	@mkdir -p $(BUILD_DIR)  
	nasm -f elf64 $< -o $(BUILD_DIR)/$(notdir $@)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm boot.elf boot.bin

.PHONY: all clean