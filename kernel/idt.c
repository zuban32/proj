#include "idt.h"
#include "kbd.h"

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
	out(PIC_M_CMD, PIC_EOI);
	// printint(scancode);
}

struct idt_entry idt_tbl[IDTSIZE];
struct idt_descr idtr = {IDTSIZE * sizeof(struct idt_entry), (uint32_t)idt_tbl};

void
load_idt(void)
{
	// printint(16, 16);
	idt_tbl->offset1 = (uint16_t)&fpexc;
	idt_tbl->selector = 0x8;
	idt_tbl->zero = 0;
	idt_tbl->type = 0x8e;
	idt_tbl->offset2 = 0;

	idt_tbl[0x21].offset1 = (uint16_t)(&kbd_hndl & 0xff);
	idt_tbl[0x21].selector = 0x8;
	idt_tbl[0x21].zero = 0;
	idt_tbl[0x21].type = 0x8e;
	idt_tbl[0x21].offset2 = (uint16_t)((&kbd_hndl >> 8) & 0xff);

	uint32_t p = (uint32_t)&idtr;
	__asm__ __volatile__("lidt (%0)"::"p"(p) );
}


