#include <hw/ata.h>
#include <console.h>
#include <cmd.h>
#include <process.h>
#include <x86_mem.h>
#include <gdt.h>
#include <registry.h>
#include <mmu.h>
#include <debug.h>
#include <fio.h>

static void idle()
{
	while(1);
}

extern "C" int init_main(uint32_t gdt_start)
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
	init_user_gdt((gdt_entry *)gdt_start);
//	RAMMap *map = (RAMMap *)0x500;
//	dump_map(map);
//	kprintf("RAM size: %d\n", get_memory_size(map));
	if(!res) {
		kprintf("Init finished\n");
	} else {
		dprintf("Init failed\n");
		idle();
	}

	char buf[20] = {0};
	File *f = fopen("/b/test_dir/fat32_test.txt");
	if(!f) {
		kprintf("File wasn't opened\n");
		idle();
	} else {
		kprintf("File opened succesfully: size = %d\n", f->get_size());
		fread(f, 0, f->get_size(), (uint8_t *)buf);
		kprintf("File data: [%s]\n", buf);

		int pid1 = create_process(fopen("/b/bin/proc_id"));
		int pid2 = create_process(fopen("/b/bin/proc_id"));
		int pid3 = create_process(fopen("/b/bin/proc_id"));
		if(pid1 == -1 || pid2 == -1 || pid3 == -1) {
			kprintf("Error creating process\n");
			idle();
		} else {
			sched_start();
		}
	}

	return 0;
}
