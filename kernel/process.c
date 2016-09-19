#include <inc/common.h>
#include <inc/process.h>
#include <inc/console.h>
#include <inc/string.h>
#include <inc/paging.h>

#define GD_UT 0x18
#define GD_UD 0x20

static int cur_max_pid = 0;
static Process process_table[MAX_PROCESS_NUM];

Process *create_process(void)
{
	if(cur_max_pid + 1 == MAX_PROCESS_NUM) {
		return NULL;
	}
	Process *res = &process_table[cur_max_pid++];
	kmemset(res, 0, sizeof(*res));
	res->status = PROC_EMPTY;
	res->pid = cur_max_pid - 1;
	res->code_start = NULL;

	return res;
}

Process *get_process_table(void)
{
	return process_table;
}

static Process *get_process(int pid)
{
	if(pid < 0 || pid > MAX_PROCESS_NUM) {
		return NULL;
	}
	return &process_table[pid];
}

Process *get_cur_process(void)
{
	return get_process(cur_max_pid - 1);
}

int load_process_code(Elf32_Ehdr *file, Process *proc)
{
	if(!is_elf(file)) {
		kprintf("Error loading ELF: it's not ELF\n");
		return 1;
	}
	Elf32_Phdr *ph = (Elf32_Phdr *)((uint8_t *)file + file->e_phoff);
	for(int i = 0; i < file->e_phnum; i++) {
		Elf32_Phdr *cur_ph = ph + i;
		if(cur_ph->p_type == 1) {
			page_alloc(cur_ph->p_vaddr);
			kmemset((void *)cur_ph->p_vaddr, 0, cur_ph->p_memsz);
			kmemcpy((void *)cur_ph->p_vaddr, (char *)file + cur_ph->p_offset, cur_ph->p_filesz);
		}
	}
	proc->code_start = (void *)file->e_entry;
	proc->status = PROC_READY;
	proc->iframe.ret_eip = file->e_entry;

	proc->iframe.ds = GD_UD | 3;
	proc->iframe.es = GD_UD | 3;
	proc->iframe.ss = GD_UD | 3;
	page_alloc(0xF0000000 - 4096 * 2);
	proc->iframe.esp = 0xF0000000 - (4096 * 2);
	proc->iframe.ret_cs = GD_UT | 3;
	// Enable interrupts while in user mode.
	proc->iframe.eflags |= 0x200;

	return 0;
}

void process_ret(Process *proc)
{
	__asm __volatile("movl %0,%%esp\n"
			"\tpopal\n"
			"\tpopl %%es\n"
			"\tpopl %%ds\n"
			"\taddl $0x8,%%esp\n" /* skip tf_trapno and tf_errcode */
			"\tiret"
			: : "g" (&(proc->iframe)) : "memory");
//	__asm __volatile(
//			"movw $0x23, %%ax\n\t"
//			"movw %%ax, %%ds\n\t"
//			"movw %%ax, %%es\n\t"
//			"movw %%ax, %%fs\n\t"
//			"movw %%ax, %%gs\n\t"
//
//			"movl %%esp, %%eax\n\t"
//			"pushl $0x23\n\t"
//			"pushl %%eax\n\t"
//			"pushf\n\t"
//			"pushl $0x1B\n\t"
//			"pushl %0\n\t"
//			"iret\n\t"::"r"(0x1000));

}
