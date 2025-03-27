bootloader:
	nasm bootsector.asm -f bin -o bootsector.bin
	dd if=bootsector.bin of=c.img bs=512 seek=0 conv=notrunc