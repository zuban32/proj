#include <inc/console.h>
#include <inc/common.h>
#include <inc/paging.h>

#define CHECKADDR 0x27ff010

extern uint32_t pgtbl[][PGS_NUM];

void
test(uint32_t type)
{
	kprintf("Testing: type = %d\n", type);
    switch (type)
    {
    case 0:
        asm("int $13\n\t");
        break;
    case 1:
		pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~(PAGE_P | PAGE_R | PAGE_U); 
		kprintf("Checking pagefault\n");
		char *x = (char *)CHECKADDR;
		char p = *x;
		// asm volatile("int $14\n\t");
		kprintf("Check again\n");
		p = *x;
		kprintf("End\n");
		break;
	case 2: 
		pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~(PAGE_P); 
		// int res;
		// asm volatile("movd %%mm2, %%eax\n\t":"=a"(res));
		// kprintf("mm2 = %x\n", res);
		asm volatile("movl $0x27ff010, %%edi\n\tmaskmovdqu %%xmm2, %%xmm1\n\t":::"memory");
		break;
    default:
        break;
    }

}