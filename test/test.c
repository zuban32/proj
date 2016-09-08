#include <inc/console.h>
#include <inc/common.h>
#include <inc/paging.h>

extern uint32_t pgtbl[][PGS_NUM];

void test(uint32_t type)
{
	int res = 0;
	switch (type) {
	case 0:
		// will fail because INT instruction doesn't push an error code
		__asm__("int $14\n\t");
		break;
	case 1:
		pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~(PAGE_P | PAGE_W | PAGE_U);
		kprintf("Checking pagefault\n");
		char *x = (char *) CHECKADDR;
		__attribute((unused)) char p = *x;
		kprintf("Check again\n");
		p = *x;
		kprintf("End\n");
		break;
	case 2: {
		pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~PAGE_P;

#ifdef TEST
		kprintf("kprintf for QEMU TB split\n");
#endif

		__asm__ __volatile__("addl $1, %1\n\t"
				"mov %0, %%edi\n\t"
				"maskmovq %%mm2, %%mm1\n\t"
				"maskmovdqu %%xmm2, %%xmm1\n\t"
				::"i"(CHECKADDR), "m"(res):"memory", "%eax"
		);

		kprintf("res = %d\nshould be: 1\n", res);
		break;
	}
	default:
		break;
	}

}
