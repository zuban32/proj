#include <process.h>
#include <console.h>
#include <util/string.h>
#include <events/mmu.h>
#include <events/pic.h>
#include <mmu.h>
#include <events/process.h>
#include <debug.h>

#define GD_UT 0x18
#define GD_UD 0x20

extern void kernel_main();

int Process::global_pid = 0;

static ProcessManager proc_mgr;

int ProcessManager::init()
{
	this->to_mmu = this->connect_to(UNIT_DRIVER, DRIVER_MMU, 0);
	this->to_pic = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 0);
	if(!this->to_mmu || !this->to_pic) {
		return 1;
	}

	for(int i = 0; i < MAX_PROCESS_NUM; i++) {
		if(i < MAX_PROCESS_NUM - 1) {
			this->process_table[i].set_next(&this->process_table[i+1]);
		}
		if(i > 0) {
			this->process_table[i].set_prev(&this->process_table[i-1]);
		}
	}
	this->process_table[0].finalize((uint32_t)&kernel_main, false);
	this->process_table[0].set_status(PROC_READY);
	return 0;
}

int ProcessManager::connect_from(Tunnel *t, int data)
{
	return 0;
}

int ProcessManager::handle(Event e, void *ret)
{
	switch(e.get_type()) {
	case E_PROC_IRQ:
		this->to_pic->transfer(this, Event(E_PIC_EOI, e.get_msg()), ret);
		if(this->sched_enabled) {
			this->sched();
		}
		break;
	default:
		break;
	}
	return 0;
}

int Process::finalize(uint32_t start, bool user)
{
	this->code_start = (void *)start;
	this->iframe.ret_eip = start;
	dprintf("Process[%d] start = %x\n", this->id, this->iframe.ret_eip);

	if(user) {
		this->iframe.ds = GD_UD | 3;
		this->iframe.es = GD_UD | 3;
		this->iframe.ss = GD_UD | 3;
		this->iframe.ret_cs = GD_UT | 3;
		uint32_t stack_top = PROCSTACKTOP(this->id);
		page_alloc(stack_top - 4096 * 2, 1);
		page_alloc(stack_top - 4096, 1);
		this->iframe.esp = stack_top;
		// Enable interrupts while in user mode.
		this->iframe.eflags |= 0x200;
	} else {
		page_alloc(start, 0);
		this->iframe.ret_cs = 0x8;
		this->iframe.ds = 0x10;
		this->iframe.es = 0x10;
		this->iframe.ss = 0x10;
		this->iframe.esp = 0x900000;
	}
	return 0;
}

int Process::load_code(File *file, uint32_t *start)
{
	Elf32_Ehdr ehdr;
	Elf32_Phdr phdr;
	fread(file, 0, sizeof(Elf32_Ehdr), (uint8_t *)&ehdr);

	if(!file) {
		kprintf("Process load: file wasn't provided\n");
		return 1;
	}
	if(!is_elf(&ehdr)) {
		kprintf("Error loading ELF: it's not ELF\n");
		return 1;
	}
	for(int i = 0; i < ehdr.e_phnum; i++) {
		fread(file, ehdr.e_phoff + i * sizeof(phdr), sizeof(phdr), (uint8_t *)&phdr);
		if(phdr.p_type == 1) {
			page_alloc(phdr.p_vaddr, 1);
			kmemset((void *)phdr.p_vaddr, 0, phdr.p_memsz);
			fread(file, phdr.p_offset, phdr.p_filesz, (unsigned char *)phdr.p_vaddr);
		}
	}
	if(start) {
		*start = ehdr.e_entry;
	}

	return 0;
}

void Process::free()
{
	this->status = PROC_DEAD;
	if(this->prev) {
		this->prev->set_next(this->next);
	}
	if(this->next) {
		this->next->set_prev(this->prev);
	}
	this->status = PROC_FREE;
}

void Process::exec()
{
	kprintf("Executing process %d from EIP=%x\n", this->id, this->iframe.ret_eip);
	this->status = PROC_RUNNING;
	__asm __volatile(
			"mov %0, %%esp\n\t"
			"popal\n\t"
			"pop %%es\n\t"
			"pop %%ds\n\t"
			"add $8, %%esp\n\t"
//			"sti\n\t"
			"iret\n\t"
			: : "g" (&(this->iframe)));
}

void Process::handle_irq(Intframe *i)
{
	if((i->ret_cs & 3) == 3) {
		kmemcpy((char *)&this->iframe, (char *)i, sizeof(*i));
		this->status = PROC_READY;
	}
}

int ProcessManager::exec_process(int pid)
{
	if(pid < 0 || pid >= MAX_PROCESS_NUM) {
		return 1;
	}
	this->cur_proc = this->process_table + pid;
	this->process_table[pid].exec();
	return 0;
}

int ProcessManager::create_process(File *file)
{
	Process *res = this->free_head;
	if(!res) {
		kprintf("No free processes available!\n");
		return -1;
	}
	res->set_status(PROC_TAKEN);
	if(res->get_next()) {
		res->get_next()->set_prev(nullptr);
	}
	this->free_head = res->get_next();

	if(!this->head) {
		this->head = res;
	} else {
		this->head->set_prev(res);
		res->set_next(this->head);
		this->head = res;
	}

	uint32_t start;
	res->load_code(file, &start);
	res->finalize(start, true);
	res->set_status(PROC_READY);
	return res->get_pid();
}

void ProcessManager::free_process(int pid)
{
	Process *freeing = this->process_table + pid;
	if(this->free_head) {
		this->free_head->set_prev(freeing);
	} else {
		this->free_head = freeing;
	}
	if(this->head == freeing) {
		this->head = freeing->get_next();
	}
	freeing->free();
	this->sched();
}

void ProcessManager::handle_irq(Intframe *i)
{
	this->cur_proc->handle_irq(i);
}

int create_process(File *f)
{
	return proc_mgr.create_process(f);
}

void exec_process(int pid)
{
	proc_mgr.exec_process(pid);
}

void free_process(int pid)
{
	proc_mgr.free_process(pid);
}

void sched_start()
{
	static bool called = false;
	if(!called) {
		called = true;
		proc_mgr.sched();
	} else {
		dprintf("Security violation: only init_main can call sched_start(), and only once!\n");
	}
}

