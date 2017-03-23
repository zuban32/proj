#include <hw/ata.h>
#include <hw/pic.h>
#include <idt.h>
#include <isr.h>
#include <console.h>
#include <abstract.h>
#include <mmu.h>

extern uintptr_t isr_handlers[];

static struct idt_entry idt_tbl[IDT_SIZE];
static struct idt_descr idtr =
		{ IDT_SIZE * sizeof(struct idt_entry), (uint32_t) idt_tbl };

InterruptUnit u_idt;

static void addISR(uint8_t ind, uint16_t selector, uint8_t type)
{
	uintptr_t addr = isr_handlers[ind];
//	kprintf("Setting ISR addr [%x] for #%d: %x\n", idt_tbl + ind, ind, addr);
	if (!addr) {
//		kprintf("FAIL: isr_addr for isr_%d = 0!\n", ind);
		return;
	}

	idt_tbl[ind].offset1 = addr & 0xffff;
	idt_tbl[ind].selector = selector;
	idt_tbl[ind].zero = 0;
	idt_tbl[ind].type = type;
	idt_tbl[ind].offset2 = (addr >> 16); // & 0xffff;
//	kprintf("Setting ISR addr for #%d: %x\n", ind, idt_tbl[ind].offset1 | (idt_tbl[ind].offset2 << 16));
}

int InterruptUnit::connect_from(Tunnel *t, int data)
{
	if(data < 0 || data > IDT_SIZE) {
		return 1;
	}
//	kprintf("IDT: tunnel[%d] created = %x\n", data, t);
	this->tuns[data] = t;
	return 0;
}

int InterruptUnit::handle(Event e, void *ret)
{
//	kprintf("IDT: handle [%d]\n", e.get_msg());
	int res = 0;
	Intframe *ifr = (Intframe *)e.get_msg();
	uint32_t irq_num = ifr->intno;
	if(irq_num >= 0 && irq_num < IDT_SIZE) {
// TODO: create new event
//		kprintf("IDT: Using tunnel %x\n", this->tuns[irq_num]);
		this->tuns[irq_num]->transfer(this, e, nullptr);
	} else {
		res = -1;
	}
//	switch(irq_num) {
//	case ISR_A
//		break;
//	default:
//		break;
//	}
	return res;
}


static void load_idt(void)
{
//	kprintf("u_idt: %x -- %x\n", &u_idt, &u_idt + 1);
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

//	kprintf("isr_handlers = %x\n", isr_handlers);
//	kprintf("idt: %x -- %x\n", ((struct idt_descr *) p)->start,
//			((struct idt_descr *) p)->start + ((struct idt_descr *) p)->size);

//	kprintf("isrKBD_start: %x\n",
//			idt_tbl[ISR_KBD].offset1 | (idt_tbl[ISR_KBD].offset2 << 16));
//	kprintf("isrATA_start: %x\n",
//				idt_tbl[ISR_ATA].offset1 | (idt_tbl[ISR_ATA].offset2 << 16));
}

int InterruptUnit::init()
{
	load_idt();
	this->inited = true;
	return 0;
}
