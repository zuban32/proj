#include "idt.h"
#include "kbd.h"
#include "pic.h"
#include "isr.h"
#include "console.h"

extern uintptr_t isr[];

struct idt_entry idt_tbl[IDTSIZE];
struct idt_descr idtr = {IDTSIZE * sizeof(struct idt_entry), (uint32_t)idt_tbl};

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
	// addISR(ISR_ZERO, 0x8, i386_GATE);
	addISR(ISR_DFAULT, 0x8, i386_GATE);
	addISR(ISR_KBD, 0x8, i386_TRAP);
	// addISR(ISR_PFAULT, 0x8, i386_GATE);

	// for(int i = 0; i < 0x1f; i++)
	// {
	// 	addISR(i, &pf_hndl, 0x8, i386_GATE);
	// }

	uint32_t p = (uint32_t)&idtr;
	__asm__ __volatile__("lidt (%0)"::"p"(p) );

}


