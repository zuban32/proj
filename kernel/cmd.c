#include <inc/console.h>
#include <inc/cmd.h>
#include <inc/string.h>
#include <inc/kbd.h>
#include <inc/idt.h>
#include <inc/isr.h>

extern char input_on;
extern char kbd_buf[];

char cmd_name[CMD_NAME_LEN + 1];
uint32_t params[2];

const char *cmd_names[CMD_NUM] = {"isr", "listisr", "test", "clear", "help"};
void (*cmds[CMD_NUM])(void) = {&cmd_isr, &cmd_listisr, &cmd_test, &cmd_clear, &cmd_help};


void 
cmd(void)
{
	// int res;

	// asm volatile("movl %%esp, %%eax\n\t":"=a"(res));
	// kprintf("esp = %x\n", res);

	// asm volatile("movl %%ebp, %%eax\n\t":"=a"(res));
	// kprintf("ebp = %x\n", res);
	kprintf("K> ");
	clear_buf();
	input_on = 1;
	while(input_on);

	uint32_t ret_addr = 0;

	asm volatile (
		"mov 0x1c(%%esp), %%eax\n\t"
		:"=a"(ret_addr)
	);

	kprintf("ret_addr = %x\n", ret_addr);

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

	asm volatile (
		"mov 0x1c(%%esp), %%eax\n\t"
		:"=a"(ret_addr)
	);

	kprintf("ret_addr = %x\n", ret_addr);

	// asm volatile("movl %%esp, %%eax\n\t":"=a"(res));
	// kprintf("esp = %x\n", res);

	// asm volatile("movl %%ebp, %%eax\n\t":"=a"(res));
	// kprintf("ebp = %x\n", res);
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