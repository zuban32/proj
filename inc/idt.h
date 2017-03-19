#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include <inc/abstract.h>
#include <inc/common.h>
#include <inc/console.h>

enum {
	IDT_SIZE = 0x81
};

#define i386_INT 	0x8E
#define i386_TRAP 	0x8F

int isr_exists(int num);

class IDT_Unit: Unit
{
	Tunnel *tuns[IDT_SIZE];
public:
	IDT_Unit(): Unit(UNIT_PHYS, PHYS_IRQ) {}
	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e);

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

