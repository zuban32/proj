#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

#include <abstract.h>
#include <isr.h>

enum {
	PGS_NUM = 1024,
	PGSIZE = 4096,
	MMU_TUNS = 16
};

#define PAGE_P (1 << 0)
#define PAGE_W (1 << 1)
#define PAGE_U (1 << 2)

#define PDX(addr) ((addr)>>22)
#define PTX(addr) ((addr)>>12 & ((PGSIZE >> 2) - 1))
#define POFF(addr) ((addr) & ~(PGSIZE - 1))

#define USTACKTOP 0xF000000
#define PROCSTACKTOP(pid) (USTACKTOP - (pid * PGSIZE))

class MMU: public Unit
{
	Tunnel *irq = nullptr;
	Tunnel *in_tuns[MMU_TUNS] = {nullptr};
	int cur_in = 0;

	uint32_t pgtbl[PGS_NUM][PGS_NUM] __attribute__ ((aligned (PGSIZE)));
	uint32_t pgdir[PGS_NUM] __attribute__ ((aligned (PGSIZE)));

public:
	MMU(): Unit(UNIT_DRIVER, DRIVER_MMU) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

	void page_alloc(uint32_t vaddr, int user);
};

void page_alloc(uint32_t vaddr, int user);

#endif

