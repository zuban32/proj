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


void idle(void)
{
	disable_sched();
	while (1)
		cmd();
}

extern "C" int kernel_main(uint32_t gdt_start)
{
//	while(1);
//	init_vesa();
//	kclear_screen();
//	while(1);

//	Call global constructors
	extern char *__init_array_start;
	extern char *__init_array_end;
//	kprintf("Init array: start = %x, end = %x\n",
//			&__init_array_start, &__init_array_end);

	for(char **p = &__init_array_start; p < &__init_array_end; p++) {
		uint32_t a = (uint32_t)p;
		(*(void (**)(void))a)();
	}

	init_ata();

	init_kbd();
	init_pic(0x20, 0x28);
	load_idt();
	init_serial();
	init_pages();
	kprintf("GDT start: %x\n", gdt_start);
	init_user_gdt((gdt_entry *)gdt_start);
	kprintf("Init finished\n");

	// test ATA read
	ata_request_readsector(0x9000/512, 3);
	while(is_bsy() || get_cur_ind() < 3);
	kprintf("ATA finished\n");

	RAMMap *map = (RAMMap *)0x500;
//	dump_map(map);
	kprintf("RAM size: %d\n", get_memory_size(map));

	Process *pr1 = create_process((Elf32_Ehdr *)get_ata_buffer()),
			*pr2 = create_process((Elf32_Ehdr *)get_ata_buffer()),
			*pr3 = create_process((Elf32_Ehdr *)get_ata_buffer());
	Process *shell = create_kernel_process(idle);
	int ret = (shell == NULL) || (pr1 == NULL) || (pr2 == NULL) || (pr3 == NULL);
	if(ret) {
		kprintf("Error loading process\n");
	} else {
		kprintf("Processes loaded and created successfully\n");
	}

	enable_sched();
	sched_yield();

	return 0;
}
