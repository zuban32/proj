KERN_OFF equ 0x8300
KERN_SECTS equ (KERNEL_SIZE/512 + 1)
BOOT2_OFF equ 0x8100
BOOT2_SECTS equ 1
TOTAL_SECTS equ (BOOT2_SECTS + KERN_SECTS)
BOOT_DRIVE equ 0x80

[org 0x7c00]

    mov bp, 0x8000
    mov sp, bp

    ;enable VESA 1024*768*24 mode
    mov ax, 0x4f02
    mov bx, 0x4118
    int 0x10

    call load_ker
    
    xor ebx, ebx
    xor bp, bp
 	mov edi, 0x500
 	mov dword[edi], 0x508
 	mov dword[edi + 4], ebx
 	add edi, 8
    mov edx, 0x534D4150
    mov eax, 0xE820
    mov ecx, 20
    int 0x15
    jc error
    mov edx, 0x534D4150
    cmp eax, edx
    ;jnz error
    test ebx, ebx
    ;jz error
    jmp jmp_tail

map_entry_read:
    mov eax, 0xE820
    mov ecx, 20
    int 0x15
    jc map_entry_end
    mov edx, 0x534D4150
jmp_tail:
    jecxz empty_entry
    mov ecx, dword[edi + 8]
    or ecx, dword[edi + 12]
    jz empty_entry
    add edi, 20
    add dword[0x504], 1
empty_entry:
	test ebx, ebx
	jne map_entry_read
map_entry_end:
    call switch_pm
    
error:
    jmp $
    
    %include "boot/gdt.asm"
    %include "boot/switch_pm.asm"

[bits 16]
    load_ker:
    mov si, dap
    mov ah, 0x42
    mov dl, BOOT_DRIVE
    int 0x13
    jc error
    ret
    
[bits 32]
begin_PM:
    ;sti

	push gdt_start
	push KERN_OFF
    call BOOT2_OFF
    
    jmp $
dap:
	db 0x10
	db 0
	dw TOTAL_SECTS
	dw BOOT2_OFF
	dw 0
	dd 1
	dd 0

    times 510 - ($ - $$) db 0
    dw 0xaa55
