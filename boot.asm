[org 0x7C00]
bits 16

start:
    cli

    ; загружаем ядро в память (0x1000)
    mov bx, 0x1000
    call load_kernel

    ; включаем A20
    in al, 0x92
    or al, 00000010b
    out 0x92, al

    ; загружаем GDT
    lgdt [gdt_descriptor]

    ; включаем protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:init_pm

; =========================
; BIOS загрузка (ТОЛЬКО 16-bit)
; =========================
load_kernel:
    mov ah, 0x02
    mov al, 15
    mov ch, 0
    mov cl, 2
    mov dh, 0
    mov dl, 0x80
    int 0x13
    ret

; =========================
bits 32
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov esp, 0x90000

    jmp 0x1000

; =========================
; GDT
gdt_start:
    dq 0x0000000000000000
    dq 0x00cf9a000000ffff
    dq 0x00cf92000000ffff
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ 0x08
DATA_SEG equ 0x10

times 510-($-$$) db 0
dw 0xAA55