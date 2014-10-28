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
	printf("%d %x\n", -500, 0xfffaa);

	pic_init(0x20, 0x28);
	printf("PIC inited\n");

	load_idt();

	while(1);
	
	return 0;
}
