#include "console.h"
#include "cmd.h"
#include "string.h"

extern char input_on;
extern char kbd_buf[];

void 
cmd(void)
{
	kprintf("K> ");
	clear_buf();
	input_on = 1;
	while(input_on);
	// printf("Cmd got: %s\n", kbd_buf);
	if(kstrcmp(kbd_buf, "isr") == 0)
		kprintf("isr will be created\n");
	else
		kprintf("Unknown command\n");
	clear_buf();
}