bootsector:
	nasm bootsector.asm -f elf32 -o bootsector.o

stage1: bootsector | c.img
	nasm switch64.asm -f elf32 -o switch64.o
	clang -m32 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c -o setup_longmode.o setup_longmode.c
	ld -m elf_i386 -T linker.ld -o final.elf bootsector.o setup_longmode.o switch64.o
	objcopy -O binary final.elf final.bin
	dd if=final.bin of=c.img bs=512 seek=0 conv=notrunc

stage2:
	clang -m64 -ffreestanding -nostdlib -target x86_64-unknown-none -c -o stage2.o stage2.c
	ld -m elf_x86_64 -T  linker.ld -o stage2.elf stage2.o
	objcopy -O binary stage2.elf stage2.bin
	dd if=stage2.bin of=c.img bs=512 seek=3 conv=notrunc # write

bootloader: stage1 stage2

run:
	qemu-system-x86_64 c.img -serial file:serial.log

debug:
	gdb -ex "target remote localhost:1234" -ex "symbol-file final.elf"
