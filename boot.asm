[org 0x7c00]

KERN_OFF equ 0x1000

    mov [boot_drive], dl

    mov bp, 0x9000
    mov sp, bp
    
    call load_ker    
    
    call switch_pm
    
    jmp $
    
    %include "disk_read.asm"
    %include "gdt.asm"
    %include "video.asm"
    %include "switch_pm.asm"

[bits 16]
    load_ker:
    mov bx, KERN_OFF
    mov dh, 15
    mov dl, [boot_drive]
    call disk_load
    ret
    
[bits 32]
begin_PM:
    
    mov ebx, pm_str
    call print_string_pm
    
    call KERN_OFF
    
    jmp $
    
    boot_drive db 0
    pm_str db "Loading kernel", 0
    
    times 510 - ($ - $$) db 0
    dw 0xaa55
