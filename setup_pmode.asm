extern _setup_lmode
bits 16
enable_pmode:
    cli 
    ; enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al

    ; load gdt
    lgdt [gdtPtr]

    ; enable pmode bit in cr0
    mov eax, cr0
    or al, 1
    mov cr0, eax

    ; ds needs to point to GDT data entry. Qemu somehow works without this.
    mov ax, 0x10
    mov ds, ax

    ; long jump
    ; this sets cs to 0x08 (GDT code entry) and jumps to C code using that
    jmp 0x08:callC

    jmp $

stage2msg:
    db 'BOOTMGR stage2 loaded',0

myGdt:
    ; null entry
    dd 0x0 ; dd double = 4 bytes
    dd 0x0

    ; code segment
    dw 0xFFFF ; limit bottom
    dw 0x0000 ; base bottom
    db 0x0    ; more base
    db 0x9A   ; access byte
    db 0xFC   ; rest of limit & flags
    db 0x0    ; rest  of base

    ; data segment
    dw 0xFFFF ; limit bottom
    dw 0x0000 ; base bottom
    db 0x0    ; more base
    db 0x92   ; access byte
    db 0xFC   ; rest of limit & flags
    db 0x0    ; rest  of base
gdt_end:

gdtPtr:
    dw gdt_end - myGdt
    dq myGdt

bits 32 ; god damn forgot to specify this
callC:
    call _setup_lmode
    jmp $
