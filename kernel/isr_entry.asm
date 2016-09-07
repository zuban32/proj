[bits 32]

%macro isr_entry 2
global isr%1_start
isr%1_start:
	pushad
	push %1
	push esp
	extern global_handler
	call global_handler
	add esp, 8
	popad
	%if %2 = 1
	add esp, 4
	%endif
	iret
%endmacro

; exceptions
isr_entry 0, 0
isr_entry 1, 0
isr_entry 2, 0
isr_entry 3, 0
isr_entry 4, 0
isr_entry 5, 0
isr_entry 6, 0
isr_entry 7, 0
isr_entry 8, 1
isr_entry 9, 0
isr_entry 10, 1
isr_entry 11, 1
isr_entry 12, 1
isr_entry 13, 1
isr_entry 14, 1
isr_entry 15, 0
isr_entry 16, 0
isr_entry 17, 1
isr_entry 18, 0
isr_entry 19, 0
isr_entry 20, 0
isr_entry 21, 0
isr_entry 22, 0
isr_entry 23, 0
isr_entry 24, 0
isr_entry 25, 0
isr_entry 26, 0
isr_entry 27, 0
isr_entry 28, 0
isr_entry 29, 0
isr_entry 30, 1
isr_entry 31, 0
; IRQs
isr_entry 32, 0
isr_entry 33, 0
isr_entry 34, 0
isr_entry 35, 0
isr_entry 36, 0
isr_entry 37, 0
isr_entry 38, 0
isr_entry 39, 0
isr_entry 40, 0
isr_entry 41, 0
isr_entry 42, 0
isr_entry 43, 0
isr_entry 44, 0
isr_entry 45, 0
isr_entry 46, 0
isr_entry 47, 0
isr_entry 48, 0

section .data

global isr_handlers
isr_handlers:
dd isr0_start
dd isr1_start
dd isr2_start
dd isr3_start
dd isr4_start
dd isr5_start
dd isr6_start
dd isr7_start
dd isr8_start
dd isr9_start
dd isr10_start
dd isr11_start
dd isr12_start
dd isr13_start
dd isr14_start
dd isr15_start
dd isr16_start
dd isr17_start
dd isr18_start
dd isr19_start
dd isr20_start
dd isr21_start
dd isr22_start
dd isr23_start
dd isr24_start
dd isr25_start
dd isr26_start
dd isr27_start
dd isr28_start
dd isr29_start
dd isr30_start
dd isr31_start
dd isr32_start
dd isr33_start
dd isr34_start
dd isr35_start
dd isr36_start
dd isr37_start
dd isr38_start
dd isr39_start
dd isr40_start
dd isr41_start
dd isr42_start
dd isr43_start
dd isr44_start
dd isr45_start
dd isr46_start
dd isr47_start
dd isr48_start
