[bits 16]
disk_load:
    push dx
    
    mov ah, 0x02
    mov al, dh
    mov ch, 0x0
    mov dh, 0x0
    mov cl, 0x2 
    
    int 0x13 
    jc disk_error
    pop dx
    cmp dh, al 
    jne disk_error
    ret
    
disk_error:
jmp $
