#include <inc/console.h>
#include <inc/common.h>
#include <inc/paging.h>

#define CHECKADDR 0x27ff010

extern uint32_t pgtbl[][PGS_NUM];

void
test(uint32_t type)
{
    kprintf("Testing: type = %d\n", type);
    int res = 0;
    switch (type)
    {
    case 0:
        asm("int $14\n\t");
        break;
    case 1:
        // pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~(PAGE_P | PAGE_R | PAGE_U);
        kprintf("Checking pagefault\n");
        char *x = (char *)CHECKADDR;
        char p = *x;
        // asm volatile("int $14\n\t");
        kprintf("Check again\n");
        p = *x;
        kprintf("End\n");
        break;
    case 2:
    {	
        // kprintf("pdx = %x, ptx = %x\n", PDX(CHECKADDR), PTX(CHECKADDR));
        pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~1;
        // int res;
        // kprintf("OK\n");
        kprintf("&res = %x\n", &res); //fail without any kprintf here  ???
        // asm volatile("movd %%mm2, %%eax\n\t":"=a"(res));
        // kprintf("mm2 = %x\n", res);
        asm volatile("add $1, %0\n\t"
        			 "mov $0x27ff010, %%edi\n\t"
        			 "maskmovq %%mm2, %%mm1\n\t"
        			 "maskmovdqu %%xmm2, %%xmm1\n\t"
        			 // "mov (%%edi), %%eax\n\t"
        			 ::"m"(res):"memory", "%eax"
        			 );

        kprintf("res = %d\n", res);
    }
    break;
    default:
        break;
    }

}
