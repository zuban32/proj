%macro isr_entry 2
global isr%1_start
isr%1_start:
	%if %2 == 0
	push 0
	%endif
	push %1
	push ds
	push es
	pushad
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	push esp
	extern global_handler
	call global_handler
	add esp, 4
	popad
	pop es
	pop ds
	add esp, 8
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
isr_entry 49, 0
isr_entry 50, 0
isr_entry 51, 0
isr_entry 52, 0
isr_entry 53, 0
isr_entry 54, 0
isr_entry 55, 0
isr_entry 56, 0
isr_entry 57, 0
isr_entry 58, 0
isr_entry 59, 0
isr_entry 60, 0
isr_entry 61, 0
isr_entry 62, 0
isr_entry 63, 0
isr_entry 64, 0
isr_entry 65, 0
isr_entry 66, 0
isr_entry 67, 0
isr_entry 68, 0
isr_entry 69, 0
isr_entry 70, 0
isr_entry 71, 0
isr_entry 72, 0
isr_entry 73, 0
isr_entry 74, 0
isr_entry 75, 0
isr_entry 76, 0
isr_entry 77, 0
isr_entry 78, 0
isr_entry 79, 0
isr_entry 80, 0
isr_entry 81, 0
isr_entry 82, 0
isr_entry 83, 0
isr_entry 84, 0
isr_entry 85, 0
isr_entry 86, 0
isr_entry 87, 0
isr_entry 88, 0
isr_entry 89, 0
isr_entry 90, 0
isr_entry 91, 0
isr_entry 92, 0
isr_entry 93, 0
isr_entry 94, 0
isr_entry 95, 0
isr_entry 96, 0
isr_entry 97, 0
isr_entry 98, 0
isr_entry 99, 0
isr_entry 100, 0
isr_entry 101, 0
isr_entry 102, 0
isr_entry 103, 0
isr_entry 104, 0
isr_entry 105, 0
isr_entry 106, 0
isr_entry 107, 0
isr_entry 108, 0
isr_entry 109, 0
isr_entry 110, 0
isr_entry 111, 0
isr_entry 112, 0
isr_entry 113, 0
isr_entry 114, 0
isr_entry 115, 0
isr_entry 116, 0
isr_entry 117, 0
isr_entry 118, 0
isr_entry 119, 0
isr_entry 120, 0
isr_entry 121, 0
isr_entry 122, 0
isr_entry 123, 0
isr_entry 124, 0
isr_entry 125, 0
isr_entry 126, 0
isr_entry 127, 0
isr_entry 128, 0

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
dd isr49_start
dd isr50_start
dd isr51_start
dd isr52_start
dd isr53_start
dd isr54_start
dd isr55_start
dd isr56_start
dd isr57_start
dd isr58_start
dd isr59_start
dd isr60_start
dd isr61_start
dd isr62_start
dd isr63_start
dd isr64_start
dd isr65_start
dd isr66_start
dd isr67_start
dd isr68_start
dd isr69_start
dd isr70_start
dd isr71_start
dd isr72_start
dd isr73_start
dd isr74_start
dd isr75_start
dd isr76_start
dd isr77_start
dd isr78_start
dd isr79_start
dd isr80_start
dd isr81_start
dd isr82_start
dd isr83_start
dd isr84_start
dd isr85_start
dd isr86_start
dd isr87_start
dd isr88_start
dd isr89_start
dd isr90_start
dd isr91_start
dd isr92_start
dd isr93_start
dd isr94_start
dd isr95_start
dd isr96_start
dd isr97_start
dd isr98_start
dd isr99_start
dd isr100_start
dd isr101_start
dd isr102_start
dd isr103_start
dd isr104_start
dd isr105_start
dd isr106_start
dd isr107_start
dd isr108_start
dd isr109_start
dd isr110_start
dd isr111_start
dd isr112_start
dd isr113_start
dd isr114_start
dd isr115_start
dd isr116_start
dd isr117_start
dd isr118_start
dd isr119_start
dd isr120_start
dd isr121_start
dd isr122_start
dd isr123_start
dd isr124_start
dd isr125_start
dd isr126_start
dd isr127_start
dd isr128_start
