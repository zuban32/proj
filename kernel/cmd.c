#include "console.h"
#include "cmd.h"
#include "string.h"

extern char input_on;
extern char kbd_buf[];

void 
cmd(void)
{
	kprintf(1, "K> ");
	clear_buf();
	input_on = 1;
	while(input_on);
	// printf("Cmd got: %s\n", kbd_buf);
	if(kstrcmp(kbd_buf, "isr") == 0)
		kprintf(1, "isr will be created\n");
	else if(kstrcmp(kbd_buf, "clear") == 0)
		kclear_screen();
	else
		kprintf(1, "Unknown command\n");
	clear_buf();
}