#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

#define PGS_NUM 1024
#define PGSIZE 4096

#define PAGE_P 1
#define PAGE_R 2
#define PAGE_U 4

#define PDX(addr) (addr)>>22
#define PTX(addr) ((addr)>>12 & ((PGSIZE >> 2) - 1))
#define POFF(addr) (addr) & ~(PGSIZE - 1)

void init_pages(void);

#endif
