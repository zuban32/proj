#include "idt.h"
#include "kbd.h"
#include "pic.h"

extern const char scancodes[];

void 
fpexc(void)
{
	printf("Division by zero");
	while(1);
}

void
kbd_hndl(void)
{
	uint8_t sc = inb(0x60);
	// printf("sc = %d\n", sc);
	if(sc > 0 && sc < 0x36)
	{
		switch(sc)
		{
			case 0x1c:
				endline();
				break;
			case 0xe:
				backspace();
				break;
			default:
				putc(scancodes[sc]);

		}
	}
	pic_sendEOI(1);
}

void
df_hndl(void)
{
	printf("Double fault");
	while(1);
}

struct idt_entry idt_tbl[IDTSIZE];
struct idt_descr idtr = {IDTSIZE * sizeof(struct idt_entry), (uint32_t)idt_tbl};

void
create_IDTentry(uint8_t ind, void * _addr, uint16_t selector, uint8_t type)
{
	uint32_t addr = (uint32_t)_addr;
	idt_tbl[ind].offset1 = addr & 0xffff;
	idt_tbl[ind].selector = selector;
	idt_tbl[ind].zero = 0;
	idt_tbl[ind].type = type;
	idt_tbl[ind].offset2 = (addr >> 16) & 0xffff;
}

void
load_idt(void)
{
	create_IDTentry(ISR_ZERO, &fpexc, 0x8, i386_GATE);
	create_IDTentry(ISR_DFAULT, &df_hndl, 0x8, i386_GATE);
	create_IDTentry(ISR_KBD, &kbd_hndl, 0x8, i386_TRAP);

	uint32_t p = (uint32_t)&idtr;
	__asm__ __volatile__("lidt (%0)"::"p"(p) );
}


