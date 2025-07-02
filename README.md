# x86_64 low level stuff

This project is for myself to learn about and tinker with x86-64 architecture. 

## Current features
- MBR bootloader (loads kernel binary from fat32 filesystem)
- 64-bit mode
- Interrupts
- Custom fonts
- Timers
- Simple round robin scheduler
- No any kind of memory management yet

## Building and installation

Run commands of each step from root of the project.

1. Create disk image image (default script uses fdisk):
    ```
    bash createdisk.sh
2. Build and install bootloader:
    ```
    cd bootloader
    make bootloader
    make install
3. Build and install kernel:
    ```
    cd kernel
    make kernel
    bash installkernel.sh
4. Run the project
    ```
    make run