#include <inc/console.h>
#include <inc/common.h>
#include <inc/serial.h>
#include <inc/graphics.h>
#include <inc/font.h>
#include <inc/kbd.h>

static int cur_x = 0;
static int cur_y = 0;
static int cur_bound = 0;

#define abs(x) ((x) > 0? x : -x)

#define DISPLAY_STRING_LEN (DISPLAY_PIXEL_WIDTH / FONT_WIDTH)
#define DISPLAY_COLUMN_LEN (DISPLAY_PIXEL_HEIGHT / FONT_HEIGHT)

int get_curx(void)
{
	return cur_x;
}

int get_cury(void)
{
	return cur_y;
}

void set_curx(int val)
{
	cur_x = val;
}

void set_cury(int val)
{
	cur_y = val;
}

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
	char *cur_buf = kbd_get_cur_buf(), *kbd_buf = kbd_get_buf();
	int off = (cur_buf > kbd_buf) ? 1 : 0;
	*(cur_buf -= off) = 0;
	set_cur_buf(cur_buf);
}

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
		if(c == '\n') {
			kendline();
		} else if(c == '\b') {
			kbackspace();
		} else if(c == ' ') {
			cur_x++;
		} else {
			draw_letter(cur_x, cur_y, get_letter_font(c - 32));
			cur_x++;
		}
		if (move_bound)
			cur_bound = cur_x;
	}
	write_serial(c);
}

