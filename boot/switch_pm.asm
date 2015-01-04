[bits 16]
switch_pm:

    cli
    
    lgdt [gdt_descriptor]
  
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    jmp CODE_SEG:prot_mode
    
[bits 32]
prot_mode:
    
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    
    mov ebp, 0x900000       ;kernel stack top
    mov esp, ebp
     
    call begin_PM
    

