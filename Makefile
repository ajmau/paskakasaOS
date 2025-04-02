all: compile install

install:
	$(MAKE) -C bootloader install
	$(MAKE) -C kernel install

compile: bootloader kernel

bootloader:
	$(MAKE) -C bootloader bootloader

kernel:
	$(MAKE) -C kernel kernel

debug:
	qemu-system-x86_64 -hda disk.img -serial file:serial.log -d int  -no-reboot -s -S

run:
	qemu-system-x86_64 -hda disk.img -serial file:serial.log -d int  -no-reboot

stage1:
	gdb -ex "target remote localhost:1234" -ex "symbol-file bootloader/bin/stage1.elf"

stage2:
	gdb -ex "target remote localhost:1234" -ex "symbol-file bootloader/bin/stage2.elf"

stage3:
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel/boot.elf"