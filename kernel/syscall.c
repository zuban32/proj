#include <inc/syscall.h>
#include <inc/console.h>

uint32_t syscall(Process *cur_proc, uint32_t num, uint32_t par1, uint32_t par2, uint32_t par3, uint32_t par4, uint32_t par5)
{
	uint32_t res = 0;
	switch(num) {
	case SYS_PUTS:
		for(int i = 0; i < par2; i++) {
			kprintf("%c", ((char *)par1)[i]);
		}
		break;
	case SYS_GETPID:
		res = cur_proc->id;
		break;
	default:
		kprintf("Syscall with num [%d] doesn't exist\n", num);
		break;
	}
	return res;
}
