[org 0x7c00]
KERN_OFF equ 0x1000
KERN_SECTS equ (KERNEL_SIZE/512 + 1)

    mov [boot_drive], dl

    mov bp, 0x9000
    mov sp, bp

    ;enable VESA 1024*768*24 mode
    mov ax, 0x4f02
    mov bx, 0x118
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
    jnz error
    test ebx, ebx
    jz error
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
    %include "boot/video.asm"
    %include "boot/switch_pm.asm"

[bits 16]
    load_ker:
    mov ah, 2
    mov al, KERN_SECTS
    xor ch, ch
    mov cl, 2
    xor dh, dh
    mov bx, KERN_OFF
    mov dl, [boot_drive]
    int 0x13
    jc .error
    ret
.error:
	jmp $
	ret
    
[bits 32]
begin_PM:
    sti
    
    mov ebx, pm_str
    call print_string_pm

    call KERN_OFF
    
    jmp $
    
    boot_drive db 0
    pm_str db "Loading kernel", 0
    times 510 - ($ - $$) db 0
    dw 0xaa55
