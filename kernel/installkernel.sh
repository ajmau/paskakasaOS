udisksctl loop-setup -f ../disk.img
MNTPATH=`udisksctl mount -b /dev/loop0p1 | awk '{print $4}'`
echo $MNTPATH
cp boot.bin $MNTPATH
udisksctl unmount -b /dev/loop0p1
udisksctl loop-delete -b /dev/loop0