#include <abstract.h>
#include <syscall.h>
#include <events/syscall.h>
#include <console.h>
#include <idt.h>
#include <process.h>
#include <events/process.h>

static uint32_t syscall(Process *cur_proc, uint32_t num, uint32_t par1, uint32_t par2, uint32_t par3, uint32_t par4, uint32_t par5)
{
	uint32_t res = 0;
	switch(num) {
	case SYS_PUTS:
		for(unsigned i = 0; i < par2; i++) {
			kprintf("%c", ((char *)par1)[i]);
		}
		break;
	case SYS_GETPID:
		res = cur_proc->get_pid();
		break;
	case SYS_EXIT:
		free_process(cur_proc->get_pid());
		break;
	default:
		kprintf("Syscall with num [%d] doesn't exist\n", num);
		break;
	}
	return res;
}

static SyscallMgr mgr;

int SyscallMgr::init()
{
	this->irq_tun = this->connect_to(UNIT_PHYS, PHYS_IRQ, 0x80);
	this->proc_tun = this->connect_to(UNIT_SUBSYSTEM, SS_PROCESS, 0);
	if(!this->irq_tun || !this->proc_tun) {
		return 1;
	}
	return 0;
}

int SyscallMgr::connect_from(Tunnel *t, int data)
{
	return 0;
}

int SyscallMgr::handle(Event e, void *ret)
{
	Intframe *iframe = (Intframe *)e.get_msg();
	switch(e.get_type()) {
	case E_SYSCALL_IRQ: {
		ProcessManager *pmgr = (ProcessManager *)this->reg->unit_lookup(UNIT_SUBSYSTEM, SS_PROCESS);
		iframe->eax = syscall(pmgr->get_cur_process(), iframe->eax, iframe->ebx, iframe->ecx, iframe->edx, iframe->esi, iframe->edi);
		break;
	}
	default:
		break;
	}
	return 0;
}
