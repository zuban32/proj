[bits 32]
global kern_start
kern_start:
push eax

mov eax, cr0
or eax, 0x2
mov cr0, eax

mov eax, cr4
or eax, 3 << 9
mov cr4, eax

mov eax, 0xffffffff
movd xmm2, eax

pop eax
extern kernel_main
call kernel_main
ret