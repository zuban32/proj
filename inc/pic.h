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

class PICDriver: public Driver
{
	Tunnel *irq_tun;
	uint8_t m1, m2;

public:
	PICDriver(): Driver(DRIVER_PIC), irq_tun(nullptr), m1(0), m2(0) {}

	int handle(Event e);

	~PICDriver();
};

void pic_sendEOI(uint8_t irq);
void init_pic(uint8_t off1, uint8_t off2);
//void pic_set_mask(uint8_t mask_m, uint8_t mask_s);

#endif

