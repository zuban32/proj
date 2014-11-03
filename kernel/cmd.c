#include "console.h"
#include "cmd.h"
#include "string.h"
#include "kbd.h"
#include "idt.h"

extern char input_on;
extern char kbd_buf[];

char cmd_name[81];
uint32_t params[2];

void 
cmd(void)
{
	kprintf(1, "K> ");
	clear_buf();
	input_on = 1;
	while(input_on);
	kprintf(1, "Cmd got: %s\n", kbd_buf);
	parse_cmd();
	if(kstrcmp(cmd_name, "isr") == 0)
	{
		kprintf(1, "isr #%d will be created\n", params[0]);
		// create_IDTentry()
	}
	else if(kstrcmp(kbd_buf, "clear") == 0)
		kclear_screen();
	else
		kprintf(1, "Unknown command\n");
	clear_buf();
}

void
parse_cmd(void)
{
	kprintf(1, "Parsing command\n");
	uint32_t num = 0;
	char *s = kbd_buf;
	char *nm = cmd_name;
	while(!k_isspace(*s) && *s)
		*nm++ = *s++;
	*nm++ = 0;
	kprintf(1, "nm = %s\n", cmd_name);

	params[0] = kstoi(++s);
}