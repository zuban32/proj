#include <inc/console.h>
#include <inc/idt.h>
#include <inc/pic.h>
#include <inc/cmd.h>
#include <inc/serial.h>
#include <inc/paging.h>
#include <inc/kbd.h>
#include <inc/graphics.h>
#include <inc/ata.h>

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

	// test ATA read
	ata_request_readsector(1, 1);
	while(is_bsy || cur_buf_ind == 0);

	if(ata_read_buffer[0] != 0x11)
		kprintf("%d\n", ata_read_buffer[0]);
	else
		kprintf("Correct value read from HDD\n");


	while (1)
		cmd();

	return 0;
}
