#include "pic.h"
#include "console.h"

#define MASK(i, n) m##i |= 1 << n
#define UNMASK(i, n) m##i &= ~(1 << n)

void
pic_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC_S_CMD, PIC_EOI);
	outb(PIC_M_CMD, PIC_EOI);
}

void 
pic_init(uint8_t off1, uint8_t off2)
{
	// __asm__("cli");
	uint8_t m1, m2;
	m1 = inb(PIC_M_DATA);
	m2 = inb(PIC_S_DATA);

	outb(PIC_M_CMD, PIC_INIT);
	outb(PIC_S_CMD, PIC_INIT);

	outb(PIC_M_DATA, 0x20);	//ICW2
	outb(PIC_S_DATA, 0x28);

	outb(PIC_M_DATA, 0x4);	//ICW3
	outb(PIC_S_DATA, 0x2);

	outb(PIC_M_DATA, ICW4_80386);	//ICW4
	outb(PIC_S_DATA, ICW4_80386);

	outb(0x4d0, 0);
	outb(0x4d1, 0);

	outb(PIC_M_CMD, 0x6b);
	outb(PIC_S_CMD, 0x6b);

	// m1 |= 0x1;
	// m1 = 0xff;
	// m2 |= 0xff;
	MASK(1, 0);
	UNMASK(1, 4);


	kprintf(1, "Masks: %b %b\n", m1, m2);

	outb(PIC_M_DATA, m1);
	outb(PIC_S_DATA, m2);
	// __asm__("sti");
}