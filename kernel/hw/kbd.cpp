#include <inc/common.h>
#include <inc/console.h>
#include <inc/kbd.h>

static uint8_t input_on = 0;
static char kbd_buf[BUF_SIZE];
static char *cur_buf = kbd_buf;

static KbdDriver kdb_driver;

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

int KbdDriver::init()
{
	this->in = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 1);
	if(!this->in) {
		return 1;
	}
	return 0;
}

int KbdDriver::connect_from(Tunnel *t, int data)
{
	if(this->out) {
		return 1;
	}
	this->out = t;
	return 0;
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

static void kbd_hndl(void)
{
	uint8_t sc = inb(0x60);
//	char *cur_buf = kbd_get_cur_buf();
//	char *kbd_buf = kbd_get_buf();

	if (sc > 0 && sc < 0x40) {
		char letter = get_scancode(sc);
		if (input_is_on()) {
			if (cur_buf - kbd_buf == BUF_SIZE) {
				set_cur_buf(kbd_buf);
			}
			if (sc != 0x1c) {
				if (sc != 0xe) {
					*cur_buf = letter;
					set_cur_buf(++cur_buf);
				}
			} else {
				*cur_buf = 0;
				set_cur_buf(++cur_buf);
				set_input_status(0);
			}
		}

		switch (sc) {
		case 0x1c:
			kendline();
			break;
		case 0xe:
			kprintf("Backspace\n");
			kbackspace();
			break;
		default:
			kputc(letter, 0);
		}
	}
//	pic_sendEOI(1);
}

int KbdDriver::handle(Event e)
{
	kbd_hndl();
	this->in->transfer(this, Event(1, 0x1));
	return 0;
}
