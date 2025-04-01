kernel:
	nasm -f elf64 interrupt_vectors.asm -o interrupt_vectors.o
	nasm -f elf64 stub.asm -o stub.o
	clang -g  -m64 -ffreestanding -nostdlib -target x86_64-unknown-none -c -o start.o start.c
	ld -m elf_x86_64 -T  linker.ld -o boot.elf start.o stub.o interrupt_vectors.o
	objcopy -O binary boot.elf boot.bin

install: kernel
	udisksctl loop-setup -f ../disk.img
	MNTPATH=$(shell udisksctl mount -b /dev/loop0p1 mnt | awk '{print $$3}')
	cp boot.bin $(MNTPATH)
	udisksctl unmount -b /dev/loop0p1
	udisksctl loop-delete -b /dev/loop0
