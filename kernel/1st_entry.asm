[bits 32]
global kern_start
kern_start:
push eax
mov eax, 65535
movd mm2, eax
pop eax
extern kernel_main
call kernel_main
ret