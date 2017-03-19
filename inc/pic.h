#ifndef PIC_H_INCLUDED
#define PIC_H_INCLUDED

#include <inc/common.h>
#include <inc/abstract.h>

#define PIC_M_CMD 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CMD 0xA0
#define PIC_S_DATA 0xA1

#define PIC_EOI 0x20

#define PIC_INIT 0x11
#define ICW4_80386 0x1

enum {
	PIC_EVENT_IRQ,
	PIC_EVENT_EOI
};

enum {
	PIC_PIT = 0x0,
	PIC_KBD,
	PIC_CSC,
	PIC_COM2,
	PIC_COM1,
	PIC_LPT2,
	PIC_FLOPPY,
	PIC_LPT1,
	PIC_CMOS,
	PIC_FREE1,
	PIC_FREE2,
	PIC_FREE3,
	PIC_MOUSE,
	PIC_FPU,
	PIC_ATA_P,
	PIC_ATA_S,
	PIC_IRQ_MAX
};

class PICDriver: public Unit
{
	Tunnel *in_tuns[16] = {nullptr};
	Tunnel *out_tuns[16] = {nullptr};
	Tunnel *port_tun = nullptr;
	uint8_t m1, m2;

public:
	PICDriver(): Unit(UNIT_DRIVER, DRIVER_PIC), m1(0), m2(0) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e);

	~PICDriver() {}
};

void pic_sendEOI(uint8_t irq);
//void pic_set_mask(uint8_t mask_m, uint8_t mask_s);

#endif

