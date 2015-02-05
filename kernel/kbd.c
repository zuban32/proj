#include <inc/console.h>
#include <inc/kbd.h>

char input_on = 0;
char kbd_buf[BUF_SIZE];
char *cur_buf = kbd_buf;

#define NO 0

const char scancodes[KEY_NUM] = { 
	NO,   0x1B, '1',  '2',  '3',  '4',  '5',  '6',	// 0x00
	'7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
	'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',	// 0x10
	'o',  'p',  '[',  ']',  '\n', NO,   'a',  's',
	'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',	// 0x20
	'\'', '`',  NO,   '\\', 'z',  'x',  'c',  'v',
	'b',  'n',  'm',  ',',  '.',  '/',	NO,   '*',	//0x30
	NO, ' '
};

void init_kbd(void)
{
	kprintf("kbd_buf: %x -- %x\n", kbd_buf, kbd_buf + BUF_SIZE);		//just debug output
	kprintf("scancodes: %x -- %x\n", scancodes, scancodes + KEY_NUM);
}

void 
clear_buf(void)
{
	char *p = kbd_buf;
	while(p <= cur_buf)
		*p++ = 0;
	cur_buf = kbd_buf;
	// printf("Buffer cleared\n");
}