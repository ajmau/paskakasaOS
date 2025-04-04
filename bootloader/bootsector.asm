[org 0x7c00]
bits 16      ; tell the assembler we want 16 bit code

extern _setup_lmode

; segment registers need to be initialized to work on real HW
mov ax, 0x0
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

; setup stack
mov sp, 0x5000

; save drive number wich is in dl after boot
mov [driveNumber], dl

lea si, [msg] ; use lea to calculate address. mov would read a value from [msg] address
call print_string

mov ah, 2
mov al, 2 ; number of sectors
mov ch, 0 ; cylinder (first = 0)
mov cl, 2 ; sector (first = 1)
mov dh, 0 ; head number
mov dl, [driveNumber]
;;;;;;;;;mov es, 0
mov bx, 0x7e00
int 0x13


mov ah, 2
mov al, 10
mov ch, 0
mov cl, 4
mov dh, 0
mov dl, [driveNumber]
mov bx, 0xd000
int 0x13

jmp 0x7e00 ; doesn't work because of org 0x7c00 [would be 0x7c00 + 0x7c00]
;jmp $
;jmp enable_pmode

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

;times 510-($-$$) db 0
;dw 0AA55h

