#include <inc/common.h>
#include <inc/console.h>
#include <inc/kbd.h>
#include <inc/pic.h>

static KBD_State state;

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

//static int kbd_irq_cond(Intframe *iframe)
//{
//	return iframe->intno == ISR_KBD;
//}
//
//static int kbd_isr_handler(Intframe *iframe)
//{
//	uint8_t sc = inb(0x60);
//		char *cur_buf = kbd_get_cur_buf();
//		char *kbd_buf = kbd_get_buf();
//
//		if (sc > 0 && sc < 0x40) {
//			char letter = get_scancode(sc);
//			if (input_is_on()) {
//				if (cur_buf - kbd_buf == BUF_SIZE) {
//					set_cur_buf(kbd_buf);
//				}
//				if (sc != 0x1c) {
//					if (sc != 0xe) {
//						*cur_buf = letter;
//						set_cur_buf(++cur_buf);
//					}
//				} else {
//					*cur_buf = 0;
//					set_cur_buf(++cur_buf);
//					set_input_status(0);
//				}
//			}
//
//			switch (sc) {
//			case 0x1c:
//				kendline();
//				break;
//			case 0xe:
//				kbackspace();
//				break;
//			default:
//				kputc(letter, 0);
//			}
//		}
//		pic_sendEOI(1);
//		return 0;
//}

int init_kbd(void)
{
	state.input_on = 0;
	state.cur_buf = state.kbd_buf;

//	kprintf("kbd_buf: %x -- %x\n", kbd_buf, kbd_buf + BUF_SIZE);		//just debug output
//	kprintf("scancodes: %x -- %x\n", scancodes, scancodes + KEY_NUM);
	return 0;
}

char get_scancode(uint8_t sc)
{
	return scancodes[sc];
}

void clear_buf(void)
{
	char *p = state.kbd_buf;
	while (p <= state.cur_buf && state.cur_buf)
		*p++ = 0;
	state.cur_buf = state.kbd_buf;
	// printf("Buffer cleared\n");
}

uint8_t input_is_on(void)
{
	return state.input_on;
}

void set_input_status(uint8_t input)
{
	state.input_on = input;
}

char *kbd_get_cur_buf(void)
{
	return state.cur_buf;
}

char *kbd_get_buf(void)
{
	return state.kbd_buf;
}

void set_cur_buf(char *buf)
{
	state.cur_buf = buf;
}
