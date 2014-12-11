[bits 32]
global pf_start
pf_start:
	pushad
	extern pf_hndl
	call pf_hndl
	popad
	add esp, 4
	iret

global gpf_start
gpf_start:
	pushad
	extern gpf_hndl
	call gpf_hndl
	popad
	iret