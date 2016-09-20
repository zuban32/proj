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

	RAMMap *map = (RAMMap *)0x500;
	dump_map(map);
	kprintf("RAM size: %d\n", get_memory_size(map));

	Process *pr = create_process();
	if(load_process_code((Elf32_Ehdr *)get_ata_buffer(), pr)) {
		kprintf("Error loading process\n");
	} else {
		kprintf("Process loaded and created successfully\n");
		process_ret(pr);
	}

	while (1)
		cmd();

	return 0;
}
