#include "pic.h"

void
pic_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC_S_CMD, PIC_EOI);
	outb(PIC_M_CMD, PIC_EOI);
}

void 
pic_remap(uint8_t off1, uint8_t off2)
{
	uint8_t m1, m2;
	m1 = inb(PIC_M_DATA);
	m2 = inb(PIC_S_DATA);

	outb(PIC_M_CMD, PIC_INIT);
	outb(PIC_S_CMD, PIC_INIT);

	outb(PIC_M_DATA, off1);	//ICW2
	outb(PIC_S_DATA, off2);

	outb(PIC_M_DATA, 0x4);	//ICW3
	outb(PIC_S_DATA, 0x2);

	outb(PIC_M_DATA, ICW4_80386);	//ICW4
	outb(PIC_S_DATA, ICW4_80386);

	m1 |= 0xff, m2 |= 0xff;

	printint(m1, 2);
	printint(m2, 2);

	outb(PIC_M_DATA, 0);
	outb(PIC_S_DATA, 0);
}