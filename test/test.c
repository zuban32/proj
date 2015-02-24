#include <inc/console.h>
#include <inc/common.h>
#include <inc/paging.h>

extern uint32_t pgtbl[][PGS_NUM];

void
test(uint32_t type)
{
    // kprintf("Testing: type = %d\n", type);
    int res = 0;
    switch (type)
    {
    case 0:
        asm("int $14\n\t");     //fails because INT instruction doesn't push an error code
        break;
    case 1:
        pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~(PAGE_P | PAGE_W | PAGE_U);
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
        pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] &= ~PAGE_P;
        // char x = *((char *)CHECKADDR);

#ifdef TEST
        kprintf("kprintf for QEMU TB split\n");
#endif

        // kprintf("");

        asm volatile("addl $1, %1\n\t"
                     "mov %0, %%edi\n\t"
                     "maskmovq %%mm2, %%mm1\n\t"
                     "maskmovdqu %%xmm2, %%xmm1\n\t"
                     // "mov (%%edi), %%eax\n\t"
                     ::"i"(CHECKADDR), "m"(res):"memory", "%eax"
                    );

        kprintf("res = %d\nshould be: 1\n", res);
        break;
    }
    default:
        break;
    }

}
