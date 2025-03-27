bits 16      ; tell the assembler we want 16 bit code

extern _entry

; setup stack
; mov sp, 0x5000

; save drive number wich is in dl after boot
mov [driveNumber], dl

lea si, [msg] ; use lea to calculate address. mov would read a value from [msg] address
call print_string

mov ah, 2
mov al, 1 ; number of sectors
mov ch, 0 ; cylinder (first = 0)
mov cl, 2 ; sector (first = 1)
mov dh, 0 ; head number
mov dl, [driveNumber]
;mov es, 0
mov bx, 0x7e00
int 0x13

;jmp [0x7e00] ; doesn't work because of org 0x7c00 [would be 0x7c00 + 0x7c00]
jmp stage2

jmp $
; si: address of string
print_string:
    lodsb ; load byte from si to al, incease si

    ; if null loaded, jump to end
    cmp al, 0
    je end

    ; print char loaded to al
    mov ah, 0x0e
    int 0x10
    jmp print_string

    end:
    ret

msg:
    db 'Hello from real mode',0x0A,0x0D,0 ; 0x0A, 0xD newline and return

driveNumber:
    times 1 db 0

times 510-($-$$) db 0
dw 0AA55h

stage2:
    lea si, [stage2msg]
    call print_string

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

    jmp 0x08:main

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
main:
    call _entry
    jmp $ 