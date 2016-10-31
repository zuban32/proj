#include <inc/common.h>
#include <inc/process.h>
#include <inc/console.h>
#include <inc/string.h>
#include <inc/paging.h>

#define GD_UT 0x18
#define GD_UD 0x20

static int cur_max_pid = 0;
static int sched_status = 0;
static Process *cur_proc = NULL;
static Process process_table[MAX_PROCESS_NUM];

int sched_enabled(void)
{
	return sched_status == 1;
}

void enable_sched(void)
{
	sched_status = 1;
}

void disable_sched(void)
{
	sched_status = 0;
}

static Process *init_process(void)
{
	Process *cur = process_table, *end = cur + MAX_PROCESS_NUM;
	Process *prev = NULL;
	while(cur < end) {
		if(cur->status == PROC_FREE) {
			// found right place
			kmemset(cur, 0, sizeof(*cur));
			if(prev) {
				if(prev->next) {
					prev->next->prev = cur;
				}
				prev->next = cur;
				cur->next = prev->next;
			}
			cur->prev = prev;
			cur->status = PROC_TAKEN;
			break;
		} else {
			prev = cur;
		}
		cur++;
	}
	if(cur == end) {
		kprintf("Panic: can't create new process - not enought space\n");
		return NULL;
	}

	Process *res = cur;
	res->status = PROC_FREE;
	res->id = cur_max_pid++;
	kprintf("Creating proc: %d\n", res->id);
	res->code_start = NULL;

	return res;
}

Process *create_process(Elf32_Ehdr *file)
{
	Process *proc = init_process();
	load_process_code(file, proc);
	return proc;
}

Process *create_kernel_process(void (*code)(void))
{
	Process *proc = init_process();
	proc->status = PROC_TAKEN;
	proc->iframe.ret_eip = (uint32_t)code;
	page_alloc(proc->iframe.ret_eip, 0);
	proc->iframe.ret_cs = 0x8;
	proc->iframe.ds = 0x10;
	proc->iframe.es = 0x10;
	proc->iframe.ss = 0x10;
	proc->iframe.esp = 0x900000;
	proc->status = PROC_READY;
	proc->iframe.eflags |= 0x200;
	return proc;
}

void free_process(Process *proc)
{
	proc->status = PROC_DEAD;
	if(proc->prev) {
		proc->prev->next = proc->next;
	}
	if(proc->next) {
		proc->next->prev = proc->prev;
	}
	proc->status = PROC_FREE;
}

Process *get_process_table(void)
{
	return process_table;
}

//static Process *get_process(int pid)
//{
//	if(pid < 0 || pid > MAX_PROCESS_NUM) {
//		return NULL;
//	}
//	return &process_table[pid];
//}

void set_cur_process(Process *proc)
{
	cur_proc = proc;
}

Process *get_cur_process(void)
{
	return cur_proc;
}

int get_max_pid(void)
{
	return cur_max_pid;
}

int load_process_code(Elf32_Ehdr *file, Process *proc)
{
	if(!proc) {
		kprintf("Process for loading us NULL\n");
		return 1;
	}
	if(!is_elf(file)) {
		kprintf("Error loading ELF: it's not ELF\n");
		return 1;
	}
	Elf32_Phdr *ph = (Elf32_Phdr *)((uint8_t *)file + file->e_phoff);
	for(int i = 0; i < file->e_phnum; i++) {
		Elf32_Phdr *cur_ph = ph + i;
		if(cur_ph->p_type == 1) {
			page_alloc(cur_ph->p_vaddr, 1);
			kmemset((void *)cur_ph->p_vaddr, 0, cur_ph->p_memsz);
			kmemcpy((char *)cur_ph->p_vaddr, (char *)file + cur_ph->p_offset, cur_ph->p_filesz);
		}
	}
	proc->code_start = (void *)file->e_entry;
	proc->iframe.ret_eip = file->e_entry;
	kprintf("ELF start = %x\n", proc->iframe.ret_eip);

	proc->iframe.ds = GD_UD | 3;
	proc->iframe.es = GD_UD | 3;
	proc->iframe.ss = GD_UD | 3;
	uint32_t stack_top = PROCSTACKTOP(proc);
	page_alloc(stack_top - 4096 * 2, 1);
	page_alloc(stack_top - 4096, 1);
	proc->iframe.esp = stack_top;
	proc->iframe.ret_cs = GD_UT | 3;
	// Enable interrupts while in user mode.
	proc->iframe.eflags |= 0x200;
	proc->status = PROC_READY;

	return 0;
}


void process_ret(Process *proc)
{
//	kprintf("Ret from proc %d\n", proc->id);
	proc->status = PROC_RUNNING;
	set_cur_process(proc);
	__asm __volatile(".intel_syntax noprefix\n\t"
			"mov esp, %0\n\t"
			"popad\n\t"
			"pop es\n\t"
			"pop ds\n\t"
			"add esp, 8\n\t"
//			"sti\n\t"
			"iret\n\t"
			".att_syntax\n\t"
			: : "g" (&proc->iframe));
}
