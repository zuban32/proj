#include <hw/pic.h>
#include <events/pic.h>
#include <console.h>
#include <isr.h>
#include <debug.h>

#define MASK(i, n) (m##i |= 1 << n)
#define UNMASK(i, n) (m##i &= ~(1 << n))

static PICDriver pic_driver;

int PICDriver::connect_from(Tunnel *t, int data)
{
	if(!t || (data < 0 || data > PIC_IRQ_MAX)) {
		return -1;
	}
	this->out_tuns[data] = t;
	return 0;
}

static void pic_sendEOI(uint8_t irq)
{
	if (irq >= 8)
		outb(PIC_S_CMD, PIC_EOI);
	outb(PIC_M_CMD, PIC_EOI);
}

int PICDriver::handle(Event e, void *ret)
{
	Intframe *ifr = (Intframe *)e.get_msg();
	switch(e.get_type()) {
	case E_PIC_IRQ:
		this->out_tuns[ifr->intno - 0x20]->transfer(this, e, nullptr);
		break;
	case E_PIC_EOI:
		pic_sendEOI(ifr->intno);
		break;
	}
	return 0;
}

static void pic_set_mask(uint8_t mask_m, uint8_t mask_s)
{
	outb(PIC_M_DATA, mask_m);
	outb(PIC_S_DATA, mask_s);
}

static void init_pic(uint8_t off1, uint8_t off2)
{
	__asm__("cli");
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

	outb(0x4d0, 0);
	outb(0x4d1, 0);

	outb(PIC_M_CMD, 0x6B);
	outb(PIC_S_CMD, 0x6B);

	UNMASK(1, 0);
	UNMASK(1, 4);
	UNMASK(2, 7);

	pic_set_mask(m1, m2);

//	kprintf("PIC inited\n");
	__asm__("sti");
}

int PICDriver::init()
{
//	this->port_tun = this->connect_to(UNIT_PHYS, PHYS_PORT, 0);
//	if(!this->port_tun) {
//		return -1;
//	}
	for(int i = 0; i < 16; i++) {
		this->in_tuns[i] = this->connect_to(UNIT_PHYS, PHYS_IRQ, 0x20 + i);
		if(!this->in_tuns[i]) {
			return -2;
		}
	}
	init_pic(0x20, 0x28);
	this->inited = true;
	return 0;
}

