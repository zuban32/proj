#include <debug.h>
#include <console.h>
#include <hw/serial.h>

#define abs(x) ((x) > 0? x : -x)

static void dputc(char c, char move_bound)
{
	write_serial(c);
}

static void dprints(char *str)
{
	while (*str)
		dputc(*str++, 1);
}


static void dprintint(int64_t c, uint32_t base, char is_u, int lz)
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
		dputc(res / mult + ((tmp > '9') ? 'a' - 0xa : '0'), 1);
		res %= mult;
	} while (mult /= base);
}

void dprintf(const char *fstr, ...)
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
				dprintint(x, 2, 1, lead_zeroes_num);
				break;
			case 'u':
				d = va_arg(p, uint64_t);
				dprintint(d, 10, 1, lead_zeroes_num);
				break;
			case 'd':
				d = va_arg(p, int32_t);
				dprintint(d, 10, 0, lead_zeroes_num);
				break;
			case 'x':
				x = va_arg(p, uint32_t);
				dprintf("0x");
				dprintint(x, 16, 1, lead_zeroes_num);
				break;
			case 'p':
				x = va_arg(p, uint64_t);
				dprintf("0x");
				dprintint(x, 16, 1, lead_zeroes_num);
				break;
			case 's':
				s = va_arg(p, char *);
				dprints(s);
				break;
			case 'c':
				c = va_arg(p, int);
				dputc(c, 1);
				break;
			default:
				break;
			}
		} else {
			dputc(*fstr, 1);
		}
	}
	va_end(p);
}
