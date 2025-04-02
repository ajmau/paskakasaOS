udisksctl loop-setup -f ../disk.img
sudo mkfs.fat -F 32 /dev/loop0p1
MNTPATH=`udisksctl mount -b /dev/loop0p1 | awk '{print $4}'`
echo $MNTPATH
cp boot.bin $MNTPATH
cp ../font.psf $MNTPATH
udisksctl unmount -b /dev/loop0p1
udisksctl loop-delete -b /dev/loop0
