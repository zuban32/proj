#include <inc/console.h>
#include <inc/idt.h>
#include <inc/pic.h>
#include <inc/cmd.h>
#include <inc/serial.h>
#include <inc/paging.h>
#include <inc/kbd.h>
#include <inc/graphics.h>
#include <inc/ata.h>
#include <inc/process.h>
#include <inc/x86_mem.h>
#include <inc/gdt.h>

int kernel_main(uintptr_t gdt_start)
{
	init_vesa();
	kclear_screen();

	init_kbd();
	init_pic(0x20, 0x28);
	load_idt();
	init_serial();
	init_pages();
	kprintf("GDT start: %x\n", gdt_start);
	init_user_gdt((gdt_entry *)gdt_start);
	kprintf("Init finished\n");

	// test ATA read
	ata_request_readsector(48, 3);
	while(is_bsy() || get_cur_ind() < 3);

	if(((uint8_t *)get_ata_buffer())[0x350] != 0x8B)
		kprintf("%x\n", ((uint8_t *)get_ata_buffer())[0x350]);
	else
		kprintf("Correct value read from HDD\n");

	Process *pr = create_process();
	if(pr == NULL) {
		kprintf("Error creating process\n");
	} else {
		load_process_code((Elf32_Ehdr *)get_ata_buffer(), pr);
	}

	kprintf("Process loaded and created successfully\n");
	process_ret(pr);

	RAMMap *map = (RAMMap *)0x500;
	kprintf("RAM size: %d\n", get_memory_size(map));

	while (1)
		cmd();

	return 0;
}
