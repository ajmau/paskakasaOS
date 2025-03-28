extern enable_pae
extern switch_to_64
extern kernel_main
bits 32

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

    ;; DO NOT DO BEFORE JUMPING TO 64BIT
    ;mov ebx, [esp + 16]
    ;lgdt[ebx]
    ;xchg bx, bx

    jmp 0x08:jee
    pop ebp
    ret


[bits 64]
jee:
    mov rax, 0x123456
    jmp $
