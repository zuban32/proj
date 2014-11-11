#include "console.h"
#include "cmd.h"
#include "string.h"
#include "kbd.h"
#include "idt.h"
#include "isr.h"

extern char input_on;
extern char kbd_buf[];
extern struct idt_entry idt_tbl[];

char cmd_name[CMD_NAME_LEN + 1];
uint32_t params[2];

const char *cmds[CMD_NUM] = {"isr", "listisr", "int", "clear", "help"};
const char *cmd_help[CMD_NUM] = {"create ISR #param0", "list all existing ISRs", "equal to asm int #param0", "clear screen", "show this help"};

#define TESTBASE 0x1000000

char *test = TESTBASE;

void 
cmd(void)
{
	kprintf(1, "K> ");
	clear_buf();
	input_on = 1;
	while(input_on);

	kprintf(1, "Cmd got: %s\n", kbd_buf);
	parse_cmd();
	if(kstrcmp(cmd_name, "isr") == 0)				//REWRITE ALL THIS SHIT
	{
		kprintf(1, "isr #%d will be created\n", params[0]);
		addISR(params[0], 0x8, i386_GATE);
	}
	else if(kstrcmp(cmd_name, "listisr") == 0)
	{
		for(int i = 0; i < IDTSIZE; i++)
			if(idt_tbl[i].offset1 || idt_tbl[i].offset2)
				kprintf(1, "%d ", i);
			kendline();
	}
	else if(kstrcmp(cmd_name, "int") == 0)
	{
		*test++ = 0xCD;
		*test++ = (char)params[0];
		asm volatile("jmp %0\n"::"r"(test - 2));
	}
	else if(kstrcmp(kbd_buf, "clear") == 0)
		kclear_screen();
	else if(kstrcmp(kbd_buf, "help") == 0)
	{
		for(int i = 0; i < CMD_NUM; i++)
			kprintf(1, "%s - %s\n", cmds[i], cmd_help[i]);
	}
	else
		kprintf(1, "Unknown command\n");
}

void
parse_cmd(void)
{
	// kprintf(1, "Parsing command\n");
	uint32_t num = 0;
	char *s = kbd_buf;
	char *nm = cmd_name;
	while(!k_isspace(*s) && *s)
		*nm++ = *s++;
	*nm++ = 0;
	// kprintf(1, "nm = %s\n", cmd_name);
	while(k_isspace(*s) && *s) s++;
	params[0] = kstoi(s);
}