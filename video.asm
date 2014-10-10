[bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0xf

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY
    .print_string_pm_loop:
    mov al, [ebx] 
    mov ah, WHITE_ON_BLACK
    
    test al, al
    jz .print_string_pm_done
    
    mov [edx], ax
    
    inc ebx
    add edx, 2
    
    jmp .print_string_pm_loop
 .print_string_pm_done:
    popa
    ret

