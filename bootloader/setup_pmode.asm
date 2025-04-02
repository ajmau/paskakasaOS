extern _setup_lmode


bits 16
enable_pmode:

    cli 


; read e820 memory map into [entries]
    lea edi, [entries]
    xor ebx, ebx
loop:
    mov edx, 0x534D4150
    mov eax, 0xe820
    mov ecx, 24 ; count
    int 0x15

    test ebx, ebx ; are all entries read?
    jz continue

    ; increase pointer and read next memory map entry
    add edi, 24 
    jmp loop

error:
    jmp $


continue:



    ;xchg bx, bx

    mov ax, 0x4F02	; set VBE mode
    mov bx, 0x4117	; VBE mode number; notice that bits 0-13 contain the mode number and bit 14 (LFB) is set and bit 15 (DM) is clear.
    int 0x10			; call VBE BIOS
    cmp ax, 0x004F	; test for error
    jne error

    xchg bx, bx
    mov ax, 0x4F01
    mov cx,  0x117;0x0117
    mov di, vbe_info_structure
    int 0x10
    cmp ax, 0x004F	; test for error
    jne error
    xchg bx, bx

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

    xchg bx, bx
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
    db 0xCF   ; rest of limit & flags
    db 0x0    ; rest  of base

    ; data segment
    dw 0xFFFF ; limit bottom
    dw 0x0000 ; base bottom
    db 0x0    ; more base
    db 0x92   ; access byte
    db 0xCF   ; rest of limit & flags
    db 0x0    ; rest  of base
gdt_end:

gdtPtr:
    dw gdt_end - myGdt
    dq myGdt

bits 32 ; god damn forgot to specify this
callC:
    lea edi, [vbe_info_structure]
    push edi
    call _setup_lmode
    jmp $

section .data

vbe_info_structure:
;;	.signature		db "VBE2"	; indicate support for VBE 2.0+
	.table_data:		resb 512

align 16
entries:
    dq 0, 0   ; First entry: 2 x uint64_t
    dd 0      ; First uint32_t
    dd 0      ; Second uint32_t (ACPI 3.0)

    dq 0, 0   ; Second entry
    dd 0
    dd 0

    dq 0, 0
    dd 0
    dd 0

    dq 0, 0
    dd 0
    dd 0

    dq 0, 0
    dd 0
    dd 0
