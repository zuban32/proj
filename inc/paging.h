#ifndef PAGING_H_INCLUDED
#define PAGING_H_INCLUDED

#include <inc/isr.h>

#define PGS_NUM 1024
#define PGSIZE 4096

#define PAGE_P 1
#define PAGE_W 1 << 1
#define PAGE_U 1 << 2

#define CHECKADDR 0x1346789

#define PDX(addr) ((addr)>>22)
#define PTX(addr) ((addr)>>12 & ((PGSIZE >> 2) - 1))
#define POFF(addr) ((addr) & ~(PGSIZE - 1))

#define USTACKTOP 0xF000000
#define PROCSTACKTOP(proc) (USTACKTOP - (proc->id * PGSIZE))

void init_pages(void);
void handle_pagefault(Intframe *iframe);
void page_alloc(uint32_t vaddr);

#endif

