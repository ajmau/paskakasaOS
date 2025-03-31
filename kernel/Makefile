kernel:
	clang -m64 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c -o start.o start.c
	ld -m elf_x86_64 -T  linker.ld -o boot.elf start.o
	objcopy -O binary nigga.elf boot.bin
