#include "console.h"
#include "cmd.h"
#include "string.h"
#include "kbd.h"
#include "idt.h"
#include "isr.h"

extern char input_on;
extern char kbd_buf[];

char cmd_name[CMD_NAME_LEN + 1];
uint32_t params[2];

const char *cmd_names[CMD_NUM] = {"isr", "listisr", "int", "clear", "help", "check"};
void (*cmds[CMD_NUM])(void) = {&cmd_isr, &cmd_listisr, &cmd_int, &cmd_clear, &cmd_help, &cmd_check};


void 
cmd(void)
{
	kprintf("K> ");
	clear_buf();
	input_on = 1;
	while(input_on);

	// kprintf(1, "Cmd got: %s\n", kbd_buf);
	parse_cmd();

	int i = 0;
	for(i = 0; i < CMD_NUM; i++)
	{
		if(kstrcmp(cmd_name, cmd_names[i]) == 0)
		{
			cmds[i]();
			break;
		}
	}
	if(i == CMD_NUM)
		kprintf("Unknown command '%s'\n", cmd_name);
}

void
parse_cmd(void)
{
	// kprintf(1, "Parsing command\n");
	uint32_t num = 0;
	char *s = kbd_buf;
	char *nm = cmd_name;
	while(k_isspace(*s) && *s) s++;
	while(!k_isspace(*s) && *s)
		*nm++ = *s++;
	*nm++ = 0;
	// kprintf(1, "nm = %s\n", cmd_name);
	while(k_isspace(*s) && *s) s++;
	params[0] = kstoi(s);
}