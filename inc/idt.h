#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include <abstract.h>
#include <util/port.h>

enum {
	IDT_SIZE 	= 0x81,

	i386_INT 	= 0x8E,
	i386_TRAP 	= 0x8F
};


class InterruptUnit: Unit
{
	Tunnel *tuns[IDT_SIZE];
public:
	InterruptUnit(): Unit(UNIT_PHYS, PHYS_IRQ) {}
	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

};

struct idt_entry
{
	uint16_t offset1;
	uint16_t selector;
	uint8_t zero;
	uint8_t type;
	uint16_t offset2;
}__attribute__ ((packed));

struct idt_descr
{
	uint16_t size;
	uint32_t start;
}__attribute__ ((packed));

#endif

