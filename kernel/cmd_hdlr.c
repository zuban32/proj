#include <inc/console.h>
#include <inc/cmd.h>
#include <inc/string.h>
#include <inc/idt.h>
#include <inc/isr.h>
#include <inc/paging.h>

extern char *cmd_names[];
extern struct idt_entry idt_tbl[];
extern uint32_t params[];

const char *cmd_helps[CMD_NUM] = 
{"create ISR #param0", 
 "list all existing ISRs", 
 "run test with #param0", 
 "clear screen", 
 "show this help"
};

void
cmd_isr(void)
{
	kprintf("isr #%d will be created\n", params[0]);
	addISR(params[0], 0x8, i386_INT);
}

void
cmd_listisr(void)
{
	for(int i = 0; i < IDTSIZE; i++)
		if(idt_tbl[i].offset1 || idt_tbl[i].offset2)
			kprintf("%d ", i);
	kendline();
}

void
cmd_test(void)
{
	// void test();
	// asm volatile("jmp %0\n"::"r"(&test));
	test(params[0]);
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
		kprintf("%s - %s\n", cmd_names[i], cmd_helps[i]);
}