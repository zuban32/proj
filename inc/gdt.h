#ifndef INC_GDT_H_
#define INC_GDT_H_

#include <stdint.h>

typedef struct
{
	unsigned limit_low:16;
	unsigned base_low : 24;
     //attribute byte split into bitfields
	unsigned accessed :1;
	unsigned read_write :1; //readable for code, writable for data
	unsigned conforming_expand_down :1; //conforming for code, expand down for data
	unsigned code :1; //1 for code, 0 for data
	unsigned always_1 :1; //should be 1 for everything but TSS and LDT
	unsigned DPL :2; //privilege level
	unsigned present :1;
     //and now into granularity
	unsigned limit_high :4;
	unsigned available :1;
	unsigned always_0 :1; //should always be 0
	unsigned big :1; //32bit opcodes for code, uint32_t stack for data
	unsigned gran :1; //1 to use 4k page addressing, 0 for byte addressing
	unsigned base_high :8;
}  __attribute__((packed)) gdt_entry;

typedef struct
{
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // everything below here is unusued now..
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;
   uint32_t cs;
   uint32_t ss;
   uint32_t ds;
   uint32_t fs;
   uint32_t gs;
   uint32_t ldt;
   uint16_t trap;
   uint16_t iomap_base;
} __attribute__((packed)) tss_entry;

void init_user_gdt(void);

#endif /* INC_GDT_H_ */
