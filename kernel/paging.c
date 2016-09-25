#include <inc/paging.h>
#include <inc/console.h>
#include <inc/common.h>

static uint32_t pgtbl[PGS_NUM][PGS_NUM] __attribute__ ((aligned (PGSIZE << 7)));
static uint32_t pgdir[PGS_NUM] __attribute__ ((aligned (PGSIZE)));

void init_pages(void)
{
	unsigned long long addr = 0;
	uint32_t i, j;
	for (i = 0; i < PGS_NUM; i++) {
		if ((uint32_t) pgdir % PGSIZE)
			kprintf("pgdir isn't aligned\n");
		if ((uint32_t) &pgtbl[i][0] % PGSIZE)
			kprintf("pgtbl addr in entry #%d isn't aligned\n", i);

		pgdir[i] = ((uint32_t) &pgtbl[i] & ~(PGSIZE - 1));

		pgdir[i] |= PAGE_W | PAGE_P | PAGE_U;
		for (j = 0; j < PGS_NUM; j++) {
			pgtbl[i][j] = (addr & ~(PGSIZE - 1));
			addr += PGSIZE;
			pgtbl[i][j] |= PAGE_W | PAGE_P | PAGE_U;
		}
	}

	kprintf("pgdir: %x -- %x\n", pgdir, pgdir + sizeof(pgdir) / sizeof(*pgdir));
	kprintf("pgtbl: %x -- %x\n", pgtbl, (uintptr_t) pgtbl + sizeof(pgtbl));

	kprintf("Pages inited\n");

	__asm__ __volatile__(
			"mov %0, %%eax\t\n"
			"mov %%eax, %%cr3\t\n"
			"mov %%cr0, %%eax\t\n"
			"or $(1 << 31), %%eax\t\n"
			"mov %%eax, %%cr0\t\n"
			::"g"(pgdir): "eax");
}

void handle_pagefault(Intframe *iframe)
{
	uint32_t err_addr = 0;
	__asm__ __volatile__("movl %%cr2, %%eax\n\t":"=a"(err_addr));
	page_alloc(err_addr, 0);
}

void page_alloc(uint32_t vaddr, int user)
{
	kprintf("Allocating page for addr %x\n", vaddr);
	pgtbl[PDX(vaddr)][PTX(vaddr)] |= PAGE_P | (user ? PAGE_U : 0) | PAGE_W;
}
