#ifndef IDT_H_INCLUDED
#define IDT_H_INCLUDED

#include <inc/abstract.h>
#include <inc/common.h>
#include <inc/console.h>

#define IDTSIZE 0x81

#define i386_INT 	0x8E
#define i386_TRAP 	0x8F

void addISR(uint8_t ind, uint16_t selector, uint8_t type);
void load_idt(void);
int isr_exists(int num);


class ISR_Socket: public Socket
{
	int num;

public:
	ISR_Socket(): num(-1) {}
	ISR_Socket(Unit *_u, int &_num): Socket(_u), num(_num) {}

	int init(Unit *_u, int _num);
	inline int get_num() { return this->num; }
	int send();
	int recv();
};

class IDT_Unit: Unit
{

public:
	ISR_Socket socks[IDTSIZE];
	IDT_Unit() {
		for(int i = 0; i < IDTSIZE; i++) {
			socks[i].init(this, i);
		}
	}
	int handle(Socket *s);

};

//IDT_Unit *get_idt_unit();

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

