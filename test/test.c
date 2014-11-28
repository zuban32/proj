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
		pgtbl[9][PGS_NUM - 1] &= ~1; 

		kprintf("Checking pagefault\n");
		char *x = (char *)CHECKADDR;
		*x = 0;
		kprintf("No pagefault\n");
		break;
    default:
        break;
    }

}