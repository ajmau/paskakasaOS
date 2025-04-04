extern enable_pae
extern switch_to_64


bits 32

PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

GDT:
    .Null: equ $ - GDT
        dq 0
    .Code: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Data: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | LONG_MODE | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Pointer:
        dw $ - GDT - 1
        dq GDT

enable_pae:
    push ebp
    mov ebp, esp

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    pop ebp
    ret

switch_to_64:
    push ebp
    mov ebp, esp

    xchg bx, bx
    cli
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, [esp + 8]
    mov cr3, eax

    mov eax, cr0
 
    or eax, 1 << 31
    mov cr0, eax

    xchg bx, bx
    lgdt[GDT.Pointer]
    mov ax, 0x10
    mov ds, ax                    ; Set the data segment to the A-register.
    mov es, ax                    ; Set the extra segment to the A-register.
    mov fs, ax                    ; Set the F-segment to the A-register.
    mov gs, ax                    ; Set the G-segment to the A-register.
    mov ss, ax

    jmp GDT.Code:jee
    pop ebp
    ret

[bits 64]
jee:

    mov rdi, [esp + 16]

    call 0xd000
