bootsector:
	nasm bootsector.asm -f elf32 -o bootsector.o

bootloader: bootsector
	nasm enable_paging.asm -f elf32 -o enable_paging.o
	clang -m32 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c -o boot.o boot.c
	ld -m elf_i386 -T linker.ld -o final.elf bootsector.o boot.o enable_paging.o
	objcopy -O binary final.elf final.bin
	dd if=final.bin of=c.img bs=512 seek=0 conv=notrunc

run:
	qemu-system-x86_64 final.bin -serial file:serial.log

debug:
	gdb -ex "target remote localhost:1234" -ex "symbol-file final.elf"
