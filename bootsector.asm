org 0x7C00   ; add 0x7C00 to label addresses
bits 16      ; tell the assembler we want 16 bit code

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
    db 'Hello from real mode',0x0A,0x0D,0

driveNumber:
    times 1 db 0

times 510-($-$$) db 0
dw 0AA55h

stage2:
    lea si, [stage2msg]
    call print_string

    jmp $

stage2msg:
    db 'BOOTMGR stage2 loaded',0

myGdt:
