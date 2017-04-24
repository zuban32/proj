#include <hw/kbd.h>
#include <hw/serial.h>
#include <events/serial.h>
#include <events/pic.h>
#include <console.h>
#include <util/string.h>
#include <util/port.h>

static SerialDriver serial_driver;

int SerialDriver::connect_from(Tunnel *t, int data)
{
	if(this->out) {
		return 1;
	}
	this->out = t;
	return 0;
}

static void init_serial()
{
	outb(COM1 + 1, 0x00);    // Disable all interrupts
	outb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(COM1 + 1, 0x00);    //                  (hi byte)
	outb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
	outb(COM1 + 1, 0x1);
}

int SerialDriver::init()
{
	this->pic = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 4);
	if(!this->pic) {
		return 1;
	}
	init_serial();
	return 0;
}

static int serial_received()
{
	return inb(COM1 + 5) & 1;
}

static char read_serial()
{
	if (serial_received() == 0)
		return -1;

	char res = inb(COM1);
	return res;
}

static int is_transmit_empty()
{
	return inb(COM1 + 5) & 0x20;
}

void write_serial(char a)
{
	while (is_transmit_empty() == 0);
	outb(COM1, a);
}

static int com_hndl(void)
{
	uint8_t c = read_serial();
	char *cur_buf = kbd_get_cur_buf();
	char *kbd_buf = kbd_get_buf();
	if (input_is_on()) {
		if (cur_buf - kbd_buf == BUF_SIZE)
			set_cur_buf(kbd_buf);
		if (c != '\n' && c != '\r') {
			if (c != 0x7f && (kisdigit(c) || kisletter(c) || kisspace(c))) {
				*cur_buf = c;
				set_cur_buf(++cur_buf);
			}
		} else {
			*cur_buf = 0;
			set_cur_buf(++cur_buf);
			set_input_status(0);
		}
		switch (c) {
		case '\r':
		case '\n':
//			kendline();
			break;
		case 0x7f:
//			kbackspace();
			break;
		default:
			kputc(c, 0);

		}
//		pic_sendEOI(4);
		return 0;
	}
	return 1;
}

int SerialDriver::handle(Event e, void *ret)
{
	switch(e.get_type()) {
	case E_SERIAL_IRQ:
		if(!com_hndl()) {
			this->pic->transfer(this, Event(E_PIC_EOI, e.get_msg()), nullptr);
		}
		// return read character
		break;
	case E_SERIAL_WRITE:
		write_serial(e.get_msg());
		break;
	default:
		break;
	}
	return 0;
}
