#include <inc/console.h>
#include <inc/cmd.h>
#include <inc/string.h>
#include <inc/idt.h>
#include <inc/paging.h>

extern char *cmd_names[];
extern uint32_t params[];

const char *cmd_helps[CMD_NUM] = { "create ISR #param0 [DEPR!]",
		"list all existing ISRs", "clear screen",
		"show this help" };

void cmd_isr(void)
{
	kprintf("[DEPRECATED!] isr #%d will be created\n", params[0]);
}

void cmd_listisr(void)
{
	for (int i = 0; i < IDT_SIZE; i++)
		if (isr_exists(i))
			kprintf("%x ", i);
	kendline();
}

void cmd_clear(void)
{
	kclear_screen();
}

void cmd_help(void)
{
	for (int i = 0; i < CMD_NUM; i++)
		kprintf("%s - %s\n", cmd_names[i], cmd_helps[i]);
}

