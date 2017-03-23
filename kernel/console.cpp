#include <console.h>
#include <events/console.h>
#include <events/serial.h>
#include <hw/serial.h>

static Console console;

#define abs(x) ((x) > 0? x : -x)

static void kprints(char *str)
{
	while (*str)
		kputc(*str++, 1);
}


static void kprintint(int64_t c, uint32_t base, char is_u, int lz)
{
	if (!is_u && c < 0) {
		kputc('-', 1);
	}

	uint64_t res = abs(c);
	uint64_t tmp = res, mult = 1, deg = 0;

	do {
		mult *= base;
		deg++;
	} while (tmp /= base);
	mult /= base;

	if (lz) {
		for (unsigned i = 0; i < lz - deg; i++)
			kputc('0', 1);
	}

	do {
		tmp = res / mult + '0';
		kputc(res / mult + ((tmp > '9') ? 'a' - 0xa : '0'), 1);
		res %= mult;
	} while (mult /= base);
}

void kprintf(const char *fstr, ...)
{
	if (!fstr)
		kprintf("Format string empty\n");       //return error code
	va_list p;
	va_start(p, fstr);
	int d;
	uint64_t x;
	char *s, c;

	char lead_zeroes_num = 0;

	for (; *fstr; fstr++) {
		if (*fstr == '%') {
			if (*(fstr + 1) == '0')
				lead_zeroes_num = (*(fstr += 2) != '0') ? *fstr - '0' : 32; //add check for digit

			switch (*++fstr) {
			case 'b':
				x = va_arg(p, uint32_t);
				kprintint(x, 2, 1, lead_zeroes_num);
				break;
			case 'u':
				d = va_arg(p, uint64_t);
				kprintint(d, 10, 1, lead_zeroes_num);
				break;
			case 'd':
				d = va_arg(p, int32_t);
				kprintint(d, 10, 0, lead_zeroes_num);
				break;
			case 'x':
				x = va_arg(p, uint32_t);
				kprintf("0x");
				kprintint(x, 16, 1, lead_zeroes_num);
				break;
			case 'p':
				x = va_arg(p, uint64_t);
				kprintf("0x");
				kprintint(x, 16, 1, lead_zeroes_num);
				break;
			case 's':
				s = va_arg(p, char *);
				kprints(s);
				break;
			case 'c':
				c = va_arg(p, int);
				kputc(c, 1);
				break;
			default:
				break;
			}
		} else {
			kputc(*fstr, 1);
		}
	}
	va_end(p);
}

void kputc(char c, char move_bound)
{
	console.handle(Event(E_CONSOLE_CHAR, c), nullptr);
}

int Console::init()
{
	this->print_tun = this->connect_to(UNIT_DRIVER, DRIVER_SERIAL, 0);
	if(!this->print_tun) {
		return 1;
	}
	return 0;
}

int Console::connect_from(Tunnel *t, int data)
{
	if(this->cur_in < 0 || this->cur_in >= MAX_CONSOLE_TUNS) {
		return 1;
	}
	this->in_tuns[this->cur_in++] = t;
	return 0;
}

int Console::handle(Event e, void *ret)
{
	switch(e.get_type()) {
	case E_CONSOLE_CHAR:
//		TODO: send exactly e deeper without creating a new event
		this->print_tun->transfer(this, Event(E_SERIAL_WRITE, e.get_msg()), ret);
		break;
	default:
		break;
	}
	return 0;
}
