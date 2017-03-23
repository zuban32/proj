#include <console.h>
#include <cmd.h>
#include <string.h>

extern char *cmd_names[];
extern uint32_t params[];

const char *cmd_helps[CMD_NUM] = { "create ISR #param0 [DEPR!]",
		"list all existing ISRs [DEPR!]", "clear screen [DEPR!]",
		"show this help" };

void cmd_isr(void)
{
	kprintf("[DEPRECATED!] isr #%d will be created\n", params[0]);
}

void cmd_listisr(void)
{
	kprintf("[DEPRECATED!]\n");
}

void cmd_clear(void)
{
	kprintf("[DEPRECATED!]\n");
}

void cmd_help(void)
{
	for (int i = 0; i < CMD_NUM; i++)
		kprintf("%s - %s\n", cmd_names[i], cmd_helps[i]);
}

