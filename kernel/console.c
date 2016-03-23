#include <inc/console.h>
#include <inc/common.h>
#include <inc/serial.h>
#include <inc/graphics.h>

int cur_x = 0;
int cur_y = 0;
static int cur_bound = 0;

extern char kbd_buf[];
extern char *cur_buf;

void kendline(void)
{
	cur_x = 0;
	cur_y++;

	write_serial('\n');
}

void kclear_screen(void)
{
	int sz = COLOR_DEPTH * DISPLAY_PIXEL_WIDTH * DISPLAY_PIXEL_HEIGHT;
	uint8_t *mem = (uint8_t *) VIDEO_MEM;
	while (sz--)
		*mem++ = 0x0;
	cur_x = 0, cur_y = 0, cur_bound = 0;
}

void kbackspace(void)
{
	if (cur_x <= cur_bound)
		return;
	if (cur_x >= 0 && cur_x < DISPLAY_STRING_LEN) {
		draw_letter(--cur_x, cur_y, NULL);
	}
	write_serial('\b');
	*(cur_buf -= (cur_buf > kbd_buf) ? 1 : 0) = 0;
}

static void kprints(char *str)
{
	while (*str)
		kputc(*str++, 1);
}

static void kprintint(int c, uint32_t base, char is_u, int lz)
{
	if (!is_u && c < 0) {
		kputc('-', 1);
	}

	uint32_t res = (uint32_t) c;
	uint32_t tmp = res, mult = 1, deg = 0;

	do {
		mult *= base;
		deg++;
	} while (tmp /= base);
	mult /= base;

	if (lz) {
		for (int i = 0; i < lz - deg; i++)
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
	uint32_t x;
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
			case 'd':
				d = va_arg(p, int);
				kprintint(d, 10, 0, lead_zeroes_num);
				break;
			case 'x':
				x = va_arg(p, uint32_t);
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
		} else if (*fstr == '\n')
			kendline();
		else
			kputc(*fstr, 1);
	}
	va_end(p);
}

// text mode function
#if 0
void kputc(char c, char move_bound) {
	if (cur_x < DISPLAY_WIDTH && cur_y < DISPLAY_HEIGHT && cur_x * cur_y >= 0) {
		uint8_t *mem = VIDEO_MEM + (cur_pos << 1);
		*mem++ = c;
		*mem++ = VGA_MODE;
		cur_pos++;
		if (move_bound)
			cur_bound = cur_pos;
	}
	write_serial(c);
}
#endif

void kputc(char c, char move_bound)
{
	if (cur_x < DISPLAY_COLUMN_LEN && cur_y < DISPLAY_STRING_LEN && cur_x >= 0
			&& cur_y >= 0) {
		draw_letter(cur_x, cur_y, (uint8_t *) letters[c - 32]);
		cur_x++;
		if (move_bound)
			cur_bound = cur_x;
	}
	write_serial(c);
}

