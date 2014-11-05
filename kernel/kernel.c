#include "kbd.h"
#include "common.h"
#include "console.h"
#include "idt.h"
#include "pic.h"
#include "string.h"
#include "cmd.h"

void 
init_pages(void)
{
	__asm__ __volatile__(
		"mov $0, %eax\n"
		 "mov %eax, %cr3\n"
		 "mov %cr0, %eax\n"
		 "or $0x10, %eax\n"
		 "mov %eax, %cr0\n"
		);
}

int
kernel_main(void)
{
	// __asm__ volatile("sti\t\n");
	kclear_screen();
	kprintf(1, "%d %x\n", -500, 0xfffaa);

	pic_init(0x20, 0x28);
	kprintf(1, "PIC inited\n");

	load_idt();

	// int x = 5 / 0;
	
	kstoi("112515");
	// init_pages();
	// kprintf(1, "Pages inited\n");

	while(1)
		cmd();
	
	return 0;
}
