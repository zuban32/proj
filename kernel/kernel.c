#include "kbd.h"
#include "common.h"
#include "console.h"
#include "idt.h"
#include "pic.h"
#include "string.h"
#include "cmd.h"
#include "serial.h"

// extern struct PageDir *pgdir;

#define PGS_NUM 1024
#define PGSIZE 4096

uint32_t pgdir[PGS_NUM];
uint32_t pgtbl[PGS_NUM][PGS_NUM];

void 
init_pages(void)
{
	uintptr_t addr = 0;
	for(int i = 0; i < PGS_NUM; i++)
	{
		pgdir[i] = ((uint32_t)&pgtbl[i][0] & ~(PGSIZE - 1));
		pgdir[i] &= 63;
		for(int j = 0; j < PGS_NUM; j++)
		{
			pgtbl[i][j] = (addr & ~(PGSIZE - 1));
			addr += PGSIZE;
			pgtbl[i][j] &= (63);
		}
	}

	__asm__ __volatile__(
		 "mov %0, %%eax\t\n"
		 "mov %%eax, %%cr3\t\n"
		 "mov %%cr0, %%eax\t\n"
		 "or $0x80000000, %%eax\t\n"
		 "mov %%eax, %%cr0\t\n"
		 ::"g"(pgdir): "eax");
	kprintf(1, "Pages inited\n");
}

int
kernel_main(void)
{
	// __asm__ volatile("sti\t\n");
	kclear_screen();
	kprintf(1, "%d %x\n", -500, 0xfffaa);

	pic_init(0x20, 0x28);
	kprintf(1, "PIC inited\n");
	load_idt();
	init_serial();

	// int x = 5 / 0;
	
	kstoi("112515");
	kprintf(1, "All inited exc pages\n");
	init_pages();
	// kprintf(1, "Pages inited\n");

	while(1)
		cmd();
	
	return 0;
}
