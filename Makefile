bootsector:
	nasm bootsector.asm -f elf32 -o bootsector.o

bootloader: bootsector
	clang -g -ffreestanding -nostdlib -target i386-unknown-none -c -o boot.o boot.c
	ld -m elf_i386 -T linker.ld -o final.elf bootsector.o boot.o
	objcopy -O binary final.elf final.bin

run:
	qemu-system-x86_64 final.bin -serial file:serial.log

debug:
	gdb -ex "target remote localhost:1234" -ex "symbol-file final.elf"