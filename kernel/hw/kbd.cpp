#include <inc/common.h>
#include <inc/console.h>
#include <inc/kbd.h>

static uint8_t input_on = 0;
static char kbd_buf[BUF_SIZE];
static char *cur_buf = kbd_buf;

static const char scancodes[] = {
NULL, 0x1B, '1', '2', '3', '4', '5',
		'6',	// 0x00
		'7', '8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
		'u',
		'i',	// 0x10
		'o', 'p', '[', ']', '\n', NULL, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k',
		'l',
		';',	// 0x20
		'\'', '`', NULL, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
		NULL, '*',	//0x30
		NULL, ' ' };

void init_kbd(void)
{
//	kprintf("kbd_buf: %x -- %x\n", kbd_buf, kbd_buf + BUF_SIZE);		//just debug output
//	kprintf("scancodes: %x -- %x\n", scancodes, scancodes + KEY_NUM);
}

char get_scancode(uint8_t sc)
{
	return scancodes[sc];
}

void clear_buf(void)
{
	char *p = kbd_buf;
	while (p <= cur_buf && cur_buf)
		*p++ = 0;
	cur_buf = kbd_buf;
	// printf("Buffer cleared\n");
}

uint8_t input_is_on(void)
{
	return input_on;
}

void set_input_status(uint8_t input)
{
	input_on = input;
}

char *kbd_get_cur_buf(void)
{
	return cur_buf;
}

char *kbd_get_buf(void)
{
	return kbd_buf;
}

void set_cur_buf(char *buf)
{
	cur_buf = buf;
}
