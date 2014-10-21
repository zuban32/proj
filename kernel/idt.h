#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include "common.h"

#define IDTSIZE 256

void load_idt(void);

struct idt_entry
{
	uint16_t offset1;
	uint16_t selector;
	uint8_t zero;
	uint8_t type;
	uint16_t offset2;
} __attribute__ ((packed));

struct idt_descr
{
	uint16_t size;
	uint32_t start;
} __attribute__ ((packed));

#endif