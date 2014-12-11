[bits 32]
global kern_start
kern_start:
extern kernel_main
call kernel_main
ret