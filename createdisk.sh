#!/bin/bash

dd if=/dev/zero of=disk.img bs=1048576 count=128
fdisk disk.img
losetup -o $[2048*512] --sizelimit $[8*1024*1024] -f
mkfs.fat -F32 /dev/loop0
mount /dev/loop0 mnt
echo "test data" > mnt/tiedosto
umount mnt