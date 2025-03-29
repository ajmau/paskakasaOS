DISKFILE=disk.img

bootsector:
	nasm -f bin bootsector.asm -o bootsector.bin
	dd if=bootsector.bin of=$(DISKFILE)  conv=notrunc bs=446 count=1

stage1: bootsector
	nasm switch64.asm -f elf32 -o switch64.o
	nasm setup_pmode.asm -f elf32 -o setup_pmode.o
	clang -m32 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c -o setup_longmode.o setup_longmode.c
	ld -m elf_i386 -T linker.ld -o stage1.elf setup_pmode.o setup_longmode.o switch64.o 
	objcopy -O binary stage1.elf stage1.bin
	dd if=final.bin of=$(DISKFILE) bs=512 seek=1 conv=notrunc

stage2: stage1
	clang -m64 -g -ffreestanding -nostdlib -target x86_64-unknown-none -c -o stage2.o stage2.c
	ld -m elf_x86_64 -T  stage2.ld -o stage2.elf stage2.o
	objcopy -O binary stage2.elf stage2.bin
	dd if=stage2.bin of=$(DISKFILE) bs=512 seek=3 conv=notrunc # write

bootloader: stage2

run:
	qemu-system-x86_64 $(DISKFILE) -serial file:serial.log

debug:
	gdb -ex "target remote localhost:1234" -ex "symbol-file stage2.elf"
