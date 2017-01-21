#include <inc/idt.h>
#include <inc/pic.h>
#include <inc/isr.h>
#include <inc/console.h>
#include <inc/paging.h>
#include <inc/ata.h>

#include <inc/abstract.h>

extern uintptr_t isr_handlers[];

static struct idt_entry idt_tbl[IDTSIZE];
static struct idt_descr idtr =
		{ IDTSIZE * sizeof(struct idt_entry), (uint32_t) idt_tbl };

int isr_exists(int num)
{
	return idt_tbl[num].offset1 || idt_tbl[num].offset2;
}

void addISR(uint8_t ind, uint16_t selector, uint8_t type)
{
	uintptr_t addr = isr_handlers[ind];
	kprintf("Setting ISR addr [%x] for #%d: %x\n", idt_tbl + ind, ind, addr);
	if (!addr) {
		kprintf("FAIL: isr_addr for isr_%d = 0!\n", ind);
		return;
	}

	idt_tbl[ind].offset1 = addr & 0xffff;
	idt_tbl[ind].selector = selector;
	idt_tbl[ind].zero = 0;
	idt_tbl[ind].type = type;
	idt_tbl[ind].offset2 = (addr >> 16); // & 0xffff;
	kprintf("Setting ISR addr for #%d: %x\n", ind, idt_tbl[ind].offset1 | (idt_tbl[ind].offset2 << 16));
}

int ISR_Socket::init(Unit *_u, int _num)
{
	this->u = _u;
	this->num = _num;
	return 0;
}

int ISR_Socket::send()
{
//	*((int *)buf) = num;
	return 0;
}

int ISR_Socket::recv()
{
	return 0;
}

extern ATADriver ata_driver;

int IDT_Unit::handle(Socket *s)
{
	int res = 0;
	Tunnel *t;
	ISR_Socket *is = static_cast<ISR_Socket *>(s);
	switch(is->get_num()) {
	case ISR_ATA:
		t = create_tunnel(s, &ata_driver.sock, sizeof(is->get_num()));
		if(t == nullptr) {
			res = -1;
		} else {
			res = t->transfer();
		}
		break;
	default:
		break;
	}
	return res;
}

// Enabling this causes weird memory bug that sets all ISRs addrs to -1
IDT_Unit u_idt;

//IDT_Unit *get_idt_unit()
//{
//	return &u_idt;
//}


void load_idt(void)
{
	kprintf("u_idt: %x -- %x\n", &u_idt, &u_idt + 1);
	addISR(ISR_DE, 0x8, i386_INT);
	addISR(ISR_DF, 0x8, i386_INT);
	addISR(ISR_KBD, 0x8, i386_TRAP);
	addISR(ISR_COM1, 0x8, i386_TRAP);
	addISR(ISR_PF, 0x8, i386_INT);
	addISR(ISR_GP, 0x8, i386_INT);
	addISR(ISR_PIT, 0x8, i386_TRAP);
	addISR(ISR_ATA, 0x8, i386_TRAP);
	addISR(ISR_SYSCALL, 0x8, i386_INT | 0x60);

	uint32_t p = (uint32_t) &idtr;
	__asm__ __volatile__("lidt (%%eax)\n\t"::"a"(p));

	kprintf("isr_handlers = %x\n", isr_handlers);
	kprintf("idt: %x -- %x\n", ((struct idt_descr *) p)->start,
			((struct idt_descr *) p)->start + ((struct idt_descr *) p)->size);

	kprintf("isrKBD_start: %x\n",
			idt_tbl[ISR_KBD].offset1 | (idt_tbl[ISR_KBD].offset2 << 16));
	kprintf("isrATA_start: %x\n",
				idt_tbl[ISR_ATA].offset1 | (idt_tbl[ISR_ATA].offset2 << 16));
}
