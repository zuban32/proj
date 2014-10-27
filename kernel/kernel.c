#include "kbd.h"
#include "common.h"
#include "console.h"
#include "idt.h"
#include "pic.h"


int
kernel_main(void)
{
	// __asm__ volatile("sti\t\n");
	clear_screen();

	// printint(723, 10);
	
	// outb(0x64, 0xf4);
	// int x = 5 / 0;

	printint(0xfffaa, 16);

	pic_remap(0x20, 0x28);
	load_idt();

	prints("PIC inited");

	while(1);
	
	return 0;
}
