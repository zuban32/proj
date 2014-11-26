#include "idt.h"
#include "kbd.h"
#include "pic.h"
#include "isr.h"
#include "console.h"

extern uintptr_t isr[];

uint32_t pgtbl[PGS_NUM][PGS_NUM] __attribute__ ((aligned (PGSIZE)));
uint32_t pgdir[PGS_NUM] __attribute__ ((aligned (PGSIZE)));

struct idt_entry idt_tbl[IDTSIZE] __attribute__ ((aligned (16 * PGSIZE)));
struct idt_descr idtr = {IDTSIZE * sizeof(struct idt_entry), (uint32_t)idt_tbl};

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
        kprintf("pgdir[%d] = %x\n", i, pgdir[i]);

        pgdir[i] |= 3;
        for (j = 0; j < PGS_NUM; j++)
        {
            // if (addr == 0x405000)
                // kprintf("addr %x mapped to %x\n", (i * PGS_NUM + j) * PGSIZE, addr);
            pgtbl[i][j] = (addr & ~(PGSIZE - 1));
            addr += PGSIZE;
            if(addr == 0x27ff000)
                pgtbl[i][j] |= 3;
            else
                pgtbl[i][j] |= 3;
        }
    }

    uint32_t x = 0x1802;
    uint32_t dir_ind = (x & (0x3ff << 22))>>22;
	uint32_t tbl_ind = (x & (0x3ff << 12))>>12;
	// eff = pgdir[eff] & (~(PGSIZE - 1));
	uint32_t *tbl_addr = (uint32_t *)(pgdir[dir_ind] & ~(PGSIZE - 1));
	kprintf("&pgtbl[%d] = %x\n", dir_ind, tbl_addr);
	tbl_addr += tbl_ind;

	uint32_t res = (*tbl_addr) & (~(PGSIZE - 1));
	res += x & 4095;
    // eff &= 
    kprintf("%x -> %x\n", x, res);

    // kprintf("pgtbl end = %x\n", &pgtbl[0] + 2);
    // while(1);

    kprintf("pgdir addr = %x\n", pgdir);
    kprintf("Pages inited\n");

    __asm__ __volatile__(
        "mov %0, %%eax\t\n"
        "mov %%eax, %%cr3\t\n"
        "mov %%cr0, %%eax\t\n"
        "or $0x80000000, %%eax\t\n"
        "mov %%eax, %%cr0\t\n"
        ::"g"(pgdir): "eax");

    kprintf("IDT addr = %x\n", idt_tbl);
    // while(1);
}

void
addISR(uint8_t ind, uint16_t selector, uint8_t type)
{
    uintptr_t addr = isr[ind];
    idt_tbl[ind].offset1 = addr & 0xffff;
    idt_tbl[ind].selector = selector;
    idt_tbl[ind].zero = 0;
    idt_tbl[ind].type = type;
    idt_tbl[ind].offset2 = (addr >> 16) & 0xffff;
}

void
load_idt(void)
{
    addISR(ISR_ZERO, 0x8, i386_GATE);
    addISR(ISR_DFAULT, 0x8, i386_GATE);
    addISR(ISR_KBD, 0x8, i386_TRAP);
    addISR(ISR_COM1, 0x8, i386_TRAP);
    addISR(ISR_PFAULT, 0x8, i386_TRAP);
    addISR(ISR_GPFAULT, 0x8, i386_GATE);

    // for(int i = 0; i < 0x1f; i++)
    // {
    //  addISR(i, &pf_hndl, 0x8, i386_GATE);
    // }

    uint32_t p = (uint32_t)&idtr;
    __asm__ __volatile__("lidt (%0)"::"p"(p) );

}
