#include <hw/ata.h>
#include <console.h>
#include <cmd.h>
#include <process.h>
#include <x86_mem.h>
#include <gdt.h>
#include <registry.h>
#include <mmu.h>
#include <debug.h>

extern "C" int kernel_main(uint32_t gdt_start)
{
//	Call global constructors
	extern char *__init_array_start;
	extern char *__init_array_end;
//	kprintf("Init array: start = %x, end = %x\n",
//			&__init_array_start, &__init_array_end);

	for(char **p = &__init_array_start; p < &__init_array_end; p++) {
		uint32_t a = (uint32_t)p;
		(*(void (**)(void))a)();
	}

	int res = get_common_registry()->init();

//	kprintf("GDT start: %x\n", gdt_start);
//	init_user_gdt((gdt_entry *)gdt_start);
//	RAMMap *map = (RAMMap *)0x500;
//	dump_map(map);
//	kprintf("RAM size: %d\n", get_memory_size(map));
	if(!res) {
//		dprintf("Init finished\n");
		kprintf("Init finished\n");
	} else {
		dprintf("Init failed\n");
	}

	while(1);
//
//	// test ATA read
//	ata_request_readsector(0x10000/512, 3);
//	while(is_bsy() || get_cur_ind() < 3);
//
//	Process *pr1 = create_process((Elf32_Ehdr *)get_ata_buffer()),
//			*pr2 = create_process((Elf32_Ehdr *)get_ata_buffer()),
//			*pr3 = create_process((Elf32_Ehdr *)get_ata_buffer());
//	int ret = (pr1 == nullptr) || (pr2 == nullptr) || (pr3 == nullptr);
//	if(ret) {
//		kprintf("Error loading process\n");
//	} else {
//		kprintf("Processes loaded and created successfully\n");
//	}
//
//	enable_sched();
//	sched_yield();

	return 0;
}
