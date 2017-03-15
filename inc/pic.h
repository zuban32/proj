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

class PICDriver: public Unit
{
	Tunnel *in_tun = nullptr;
	Tunnel *irq_tun = nullptr;
	Tunnel *port_tun = nullptr;
	uint8_t m1, m2;

public:
	PICDriver(): Unit(UNIT_DRIVER, DRIVER_PIC), m1(0), m2(0) {}

	int init();
	int connect_from(Tunnel *t);
	int handle(Event e);

	~PICDriver() {}
};

void pic_sendEOI(uint8_t irq);
void init_pic(uint8_t off1, uint8_t off2);
//void pic_set_mask(uint8_t mask_m, uint8_t mask_s);

#endif

