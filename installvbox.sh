#/bin/bash

VBoxManage createmedium disk --filename vbox.vdi --size 64 --format VDI
VBoxManage clonehd vbox.vdi vbox.img --format RAW

fdisk vbox.img

udisksctl loop-setup -f vbox.img
sudo mkfs.fat -F32 /dev/loop0p1

sudo dd if=bootloader/bin/bootsector.bin of=/dev/loop0 conv=notrunc bs=446 count=1
sudo dd if=bootloader/bin/stage1.bin of=/dev/loop0 conv=notrunc bs=512 seek=1
sudo dd if=bootloader/bin/stage2.bin of=/dev/loop0 conv=notrunc bs=512 seek=3

MNTPATH=`udisksctl mount -b /dev/loop0p1 | awk '{print $4}'`
echo $MNTPATH
cp kernel/boot.bin $MNTPATH
cp font.psf $MNTPATH

udisksctl unmount -b /dev/loop0p1
udisksctl loop-delete -b /dev/loop0


VBoxManage convertfromraw vbox.img boot.vdi --format VDI
