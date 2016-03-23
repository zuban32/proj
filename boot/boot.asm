[org 0x7c00]
KERN_OFF equ 0x1000

    mov [boot_drive], dl

    mov bp, 0x9000
    mov sp, bp

    ;enable VESA 1024*768*24 mode
    mov ax, 0x4f02
    mov bx, 0x118
    int 0x10

    call load_ker    
    
    call switch_pm
    
    jmp $
    
    %include "boot/disk_read.asm"
    %include "boot/gdt.asm"
    %include "boot/video.asm"
    %include "boot/switch_pm.asm"

[bits 16]
    load_ker:
    mov bx, KERN_OFF
    mov dh, 50
    mov dl, [boot_drive]
    call disk_load
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
