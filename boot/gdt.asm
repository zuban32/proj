[bits 32]
gdt_start:

gdt_null:
dd 0x0
dd 0x0

gdt_kernel_code:
dw 0xffff
dw 0x0
db 0x0
db 10011010b
db 11101111b
db 0x0

gdt_kernel_data:
dw 0xffff
dw 0x0
db 0x0
db 10010010b
db 11101111b
db 0x0

gdt_user_code:
dw 0xffff
dw 0x0
db 0x0
db 11111010b
db 11101111b
db 0x0

gdt_user_data:
dw 0xffff
dw 0x0
db 0x0
db 11110010b
db 11101111b
db 0x0

tss:
dw 0xffff
dw 0x7E00
db 0
db 0x89
db 01001111b
db 0

gdt_end:

gdt_descriptor:
dw gdt_end - gdt_start - 1
dd gdt_start

CODE_SEG equ gdt_kernel_code - gdt_start
DATA_SEG equ gdt_kernel_data - gdt_start
