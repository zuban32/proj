#include "idt.h"
#include "kbd.h"
#include "pic.h"

void 
fpexc(void)
{
	prints("Division by zero");
	while(1);
}

void
kbd_hndl(void)
{
	prints("kbd");
	uint8_t scancode = inb(0x60);
	// outb(PIC_M_CMD, PIC_EOI);
	printint(scancode, 16);
	pic_sendEOI(1);
}

void
df_hndl(void)
{
	prints("Double fault");
	while(1);
}

struct idt_entry idt_tbl[IDTSIZE];
struct idt_descr idtr = {IDTSIZE * sizeof(struct idt_entry), (uint32_t)idt_tbl};

void
set_IDTentry(uint8_t ind, uint32_t addr, uint16_t selector, uint8_t type)
{
	idt_tbl[ind].offset1 = addr & 0xffff;
	idt_tbl[ind].selector = selector;
	idt_tbl[ind].zero = 0;
	idt_tbl[ind].type = type;
	idt_tbl[ind].offset2 = (addr >> 16) & 0xffff;
}

void
load_idt(void)
{
	set_IDTentry(0x0, &fpexc, 0x8, 0x8e);
	set_IDTentry(0x8, &df_hndl, 0x8, 0x8e);
	set_IDTentry(0x21, &kbd_hndl, 0x8, 0x8f);

	uint32_t p = (uint32_t)&idtr;
	__asm__ __volatile__("lidt (%0)"::"p"(p) );
}


