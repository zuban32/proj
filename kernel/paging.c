#include "paging.h"
#include "console.h"
#include "common.h"

uint32_t pgtbl[PGS_NUM][PGS_NUM] __attribute__ ((aligned (PGSIZE)));
uint32_t pgdir[PGS_NUM] __attribute__ ((aligned (PGSIZE)));

void
init_pages(void)
{
    unsigned long long addr = 0;
    uint32_t i, j;
    for (i = 0; i < 10; i++)
    {
        if ((uint32_t)pgdir % PGSIZE)
            kprintf("pgdir isn't aligned\n");
        if ((uint32_t)&pgtbl[i][0] % PGSIZE)
            kprintf("pgtbl addr in entry #%d isn't aligned\n", i);
        // kprintf("%x\n", &pgtbl);
        pgdir[i] = ((uint32_t)&pgtbl[i] & ~(PGSIZE - 1));
        // kprintf("pgdir[%d] = %x\n", i, pgdir[i]);

        pgdir[i] |= 3;
        for (j = 0; j < PGS_NUM; j++)
        {
            // if (addr == 0x405000)
                // kprintf("addr %x mapped to %x\n", (i * PGS_NUM + j) * PGSIZE, addr);
            pgtbl[i][j] = (addr & ~(PGSIZE - 1));
            addr += PGSIZE;
            pgtbl[i][j] |= 3;
        }
    }

    // kprintf("pgdir addr = %x\n", pgdir);
    kprintf("Pages inited\n");

    __asm__ __volatile__(
        "mov %0, %%eax\t\n"
        "mov %%eax, %%cr3\t\n"
        "mov %%cr0, %%eax\t\n"
        "or $(1 << 31), %%eax\t\n"
        "mov %%eax, %%cr0\t\n"
        ::"g"(pgdir): "eax");
}