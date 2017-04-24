#include <isr.h>
#include <mmu.h>
#include <console.h>
#include <util/string.h>
#include <process.h>
#include <syscall.h>
#include <idt.h>
#include <debug.h>

const char *exception_names[] = {
		"Divide error",
		"Debug Exception",
		"NMI Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalide Opcode",
		"Coprocessor Not Available",
		"Double Fault",
		"Coprocessor segment overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack-Segment Fault",
		"General Protection",
		"Page Fault",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine Check",
		"SIMD Floating-Point Exception",
		"Virtualization Exception"
};

const char *eflags_flagnames[] = {
		"CF",
		nullptr,
		"PF",
		nullptr,
		"AF",
		nullptr,
		"ZF",
		"SF",
		"TF",
		"IF",
		"DF",
		"OF",
		"IOPL1",
		"IOPL2",
		"NT",
		nullptr,
		"RF",
		"VM",
		"AC",
		"VIF",
		"VIP",
		"ID"
};

static void print_intframe(Intframe *iframe)
{
	dprintf("\n-------------------------\n");
	dprintf("INTFRAME\n%d - ", iframe->intno);
	if (iframe->intno < 21 && iframe->intno != 9
			&& iframe->intno != 15)
		dprintf("%s\n", exception_names[iframe->intno]);
	else
		dprintf("User-Defined Interrupt\n");
	dprintf("ret_eip = %x\nret_cs = %x\neflags = %00b\n", iframe->ret_eip,
			iframe->ret_cs, iframe->eflags);

	if(iframe->intno == ISR_GP) {
		dprintf("ds = %x\n", iframe->ds);
		dprintf("es = %x\n", iframe->es);
		dprintf("ss = %x\n", iframe->ss);
	}

	dprintf("[ ");   //just like gdb shows enable flags
	for (int i = 0; i < 22; i++) {
		if (i == 12) {
			dprintf("IOPL=%d ",
					2 * (iframe->eflags & (1 << 13))
							+ (iframe->eflags & (1 << 12)));
			i++;
		}
		if (eflags_flagnames[i] && (iframe->eflags & (1 << i)))
			dprintf("%s ", eflags_flagnames[i]);
	}
	dprintf("]\n");

	dprintf("err_code = %03b\n", iframe->err_code & 0x7);
	if (iframe->intno == ISR_PF) {
		dprintf((!(iframe->err_code & PAGE_U)) ? "kernel " : "user ");
		dprintf((!(iframe->err_code & PAGE_W)) ? "read " : "write ");
		dprintf((!(iframe->err_code & PAGE_P)) ? "non-present\n" : "present\n");

		uint32_t err_addr = 0;
		__asm__ __volatile__("movl %%cr2, %%eax\n\t":"=a"(err_addr));
		dprintf("fault addr = %x\n", err_addr);
	}

	dprintf("\n-------------------------\n\n");
}

extern InterruptUnit u_idt;

extern "C" void global_handler(Intframe *iframe)
{
	if(iframe->intno < ISR_PF) {
		print_intframe(iframe);
	}

	u_idt.handle(Event(0, (uint32_t)iframe), nullptr);

}

