#!/bin/bash

# Create full size default partition with fdisk
# Partition type 0c
# Use MBR

dd if=/dev/zero of=disk.img bs=1048576 count=256
fdisk disk.img