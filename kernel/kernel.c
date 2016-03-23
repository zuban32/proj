#include <inc/console.h>
#include <inc/idt.h>
#include <inc/pic.h>
#include <inc/cmd.h>
#include <inc/serial.h>
#include <inc/paging.h>
#include <inc/kbd.h>
#include <inc/graphics.h>

int kernel_main(void)
{
	init_vesa();
	kclear_screen();

	init_kbd();
	pic_init(0x20, 0x28);
	load_idt();
	init_serial();
	init_pages();
	kprintf("Init finished\n");

	while (1)
		cmd();

	return 0;
}

