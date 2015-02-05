[bits 32]

%macro isr_entry 2
global %1_start
%1_start:
	pushad
	extern %1_hndl
	call %1_hndl
	popad
	%if %2 = 1
	add esp, 4
	%endif
	iret
%endmacro

isr_entry pf, 1
isr_entry gpf, 0
isr_entry kbd, 0
isr_entry com, 0