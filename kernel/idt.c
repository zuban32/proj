#include <inc/idt.h>
#include <inc/pic.h>
#include <inc/isr.h>
#include <inc/console.h>
#include <inc/paging.h>
#include <inc/graphics.h>

extern uintptr_t isr_handlers[];

struct idt_entry idt_tbl[IDTSIZE] __attribute__ ((aligned (16 * PGSIZE)));
struct idt_descr idtr =
		{ IDTSIZE * sizeof(struct idt_entry), (uint32_t) idt_tbl };

void addISR(uint8_t ind, uint16_t selector, uint8_t type)
{
	uintptr_t addr = isr_handlers[ind];
	if (!addr) {
		kprintf("FAIL: isr_addr for isr_%d = 0!\n", ind);
		return;
	}

	idt_tbl[ind].offset1 = addr & 0xffff;
	idt_tbl[ind].selector = selector;
	idt_tbl[ind].zero = 0;
	idt_tbl[ind].type = type;
	idt_tbl[ind].offset2 = (addr >> 16); // & 0xffff;
}

void load_idt(void)
{
	addISR(ISR_DE, 0x8, i386_INT);
	addISR(ISR_DF, 0x8, i386_INT);
	addISR(ISR_KBD, 0x8, i386_TRAP);
	addISR(ISR_COM1, 0x8, i386_TRAP);
	addISR(ISR_PF, 0x8, i386_INT);
	addISR(ISR_GP, 0x8, i386_INT);

	uint32_t p = (uint32_t) &idtr;
	__asm__ __volatile__("lidt (%%eax)\n\t"::"a"(p));

	kprintf("idt: %x -- %x\n", ((struct idt_descr *) p)->start,
			((struct idt_descr *) p)->start + ((struct idt_descr *) p)->size);

	kprintf("isrKBD_start: %x\n",
			idt_tbl[ISR_KBD].offset1 | (idt_tbl[ISR_KBD].offset2 << 16));
}
