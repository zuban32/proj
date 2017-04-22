#include <console.h>
#include <mmu.h>
#include <util/port.h>
#include <debug.h>

static MMU mmu;

int MMU::init()
{
	this->irq = this->connect_to(UNIT_PHYS, PHYS_IRQ, 14);
	uint32_t addr = 0;
	uint32_t i, j;
	for (i = 0; i < PGS_NUM; i++) {
		if ((uint32_t) pgdir % PGSIZE) {
//			kprintf("pgdir isn't aligned\n");
		}
		if ((uint32_t) &pgtbl[i][0] % PGSIZE) {
//			kprintf("pgtbl addr in entry #%d isn't aligned\n", i);
		}

		pgdir[i] = ((uint32_t) &pgtbl[i]);

		pgdir[i] |= PAGE_W | PAGE_P;
		for (j = 0; j < PGS_NUM; j++) {
			pgtbl[i][j] = addr;
			addr += PGSIZE;
			pgtbl[i][j] |= PAGE_W | PAGE_P;
		}
	}

//	kprintf("pgdir: %x -- %x\n", pgdir, pgdir + sizeof(pgdir) / sizeof(*pgdir));
//	kprintf("pgtbl: %x -- %x\n", pgtbl, (uintptr_t) pgtbl + sizeof(pgtbl));

//	kprintf("Pages inited\n");

	__asm__ __volatile__(
			"mov %0, %%eax\t\n"
			"mov %%eax, %%cr3\t\n"
			"mov %%cr0, %%eax\t\n"
			"or $(1 << 31), %%eax\t\n"
			"mov %%eax, %%cr0\t\n"
			::"g"(pgdir): "eax");
	return 0;
}

int MMU::connect_from(Tunnel *t, int data)
{
	if(this->cur_in < 0 || this->cur_in >= MMU_TUNS) {
		return 1;
	}
	this->in_tuns[this->cur_in++] = t;
	return 0;
}

int MMU::handle(Event e, void *ret)
{
	Intframe *iframe = (Intframe *)e.get_msg();
	uint32_t err_addr = 0;
	__asm__ __volatile__("movl %%cr2, %%eax\n\t":"=a"(err_addr));
	kprintf("PF: %x(%s)\n", err_addr, (iframe->err_code & PAGE_U) ? "user" : "kernel");
	this->page_alloc(err_addr, iframe->err_code & PAGE_U);
	return 0;
}

void MMU::page_alloc(uint32_t vaddr, int user)
{
	dprintf("Allocating page for addr %x\n", vaddr);
	pgdir[PDX(vaddr)] |= PAGE_P | PAGE_W;
	pgtbl[PDX(vaddr)][PTX(vaddr)] |= PAGE_P | PAGE_W;

	if(user) {
		pgdir[PDX(vaddr)] |= PAGE_U;
		pgtbl[PDX(vaddr)][PTX(vaddr)] |= PAGE_U;
	}
}

void page_alloc(uint32_t vaddr, int user)
{
	mmu.page_alloc(vaddr, user);
}

