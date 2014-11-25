#include "console.h"
#include "cmd.h"
#include "string.h"
#include "kbd.h"
#include "idt.h"
#include "isr.h"

extern char *cmd_names[];
extern struct idt_entry idt_tbl[];
extern uint32_t params[];

const char *cmd_helps[CMD_NUM] = 
{"create ISR #param0", 
 "list all existing ISRs", 
 "interrupts with #param0", 
 "clear screen", 
 "show this help",
 "check pagefault"
};

#define INT_INSTR 0xCD

void
cmd_isr(void)
{
	kprintf(1, "isr #%d will be created\n", params[0]);
	addISR(params[0], 0x8, i386_GATE);
}

void
cmd_listisr(void)
{
	for(int i = 0; i < IDTSIZE; i++)
		if(idt_tbl[i].offset1 || idt_tbl[i].offset2)
			kprintf(1, "%d ", i);
	kendline();
}

void
cmd_int(void)
{
	char *test = (char *)TESTBASE;
	*test++ = INT_INSTR;
	*test++ = (char)params[0];
	asm volatile("jmp %0\n"::"r"(test - 2));
}

void
cmd_clear(void)
{
	kclear_screen();
}

void
cmd_help(void)
{
	for(int i = 0; i < CMD_NUM; i++)
		kprintf(1, "%s - %s\n", cmd_names[i], cmd_helps[i]);
}

extern uint32_t pgtbl[][PGS_NUM];

void
cmd_check(void)
{
	pgtbl[9][PGS_NUM - 1] &= ~1; 
	kprintf(1, "Checking pagefault\n");
	char *x = (char *)CHECKADDR;
	*x = 0;
}