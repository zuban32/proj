#include <inc/console.h>
#include <inc/cmd.h>
#include <inc/string.h>
#include <inc/kbd.h>
#include <inc/idt.h>
#include <inc/isr.h>

char cmd_name[CMD_NAME_LEN + 1];
uint32_t params[2];

const char *cmd_names[CMD_NUM] = { "isr", "listisr", "clear", "help" };
void (*cmds[CMD_NUM])(
		void) = {cmd_isr, cmd_listisr, cmd_clear, cmd_help };

void cmd(void)
{
	kprintf("K> ");
	clear_buf();
	set_input_status(1);
	while (input_is_on());

	parse_cmd();

	int i = 0;
	for (i = 0; i < CMD_NUM; i++) {
		if (kstrcmp(cmd_name, cmd_names[i]) == 0) {
			cmds[i]();
			break;
		}
	}
	if (i == CMD_NUM)
		kprintf("Unknown command '%s'\n", cmd_name);
}

void parse_cmd(void)
{
	char *s = kbd_get_buf();
	char *nm = cmd_name;
	while (kisspace(*s) && *s)
		s++;
	while (!kisspace(*s) && *s)
		*nm++ = *s++;
	*nm++ = 0;
	while (kisspace(*s) && *s)
		s++;
	params[0] = kstoi(s);
}

