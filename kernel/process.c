#include <inc/common.h>
#include <inc/process.h>
#include <inc/console.h>
#include <inc/string.h>

static int cur_max_pid = 0;
static Process process_table[MAX_PROCESS_NUM];

Process *create_process(void)
{
	if(cur_max_pid + 1 == MAX_PROCESS_NUM) {
		return NULL;
	}
	Process *res = &process_table[cur_max_pid++];
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
			kmemset((void *)cur_ph->p_vaddr, 0, cur_ph->p_memsz);
			kmemcpy((void *)cur_ph->p_vaddr, (char *)file + cur_ph->p_offset, cur_ph->p_filesz);
		}
	}
	proc->code_start = (void *)file->e_entry;
	proc->status = PROC_READY;
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
			: : "g" (&proc->iframe) : "memory");
}
