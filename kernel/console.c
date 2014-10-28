#include "console.h"
#include "common.h"

void 
endline(void)
{
	cur_pos = (DISPLAY_WIDTH << 1) * (cur_pos / (DISPLAY_WIDTH << 1) + 1);
}

void
clear_screen(void)
{
	int sz = DISPLAY_WIDTH * DISPLAY_HEIGHT;
	char *mem = VGA_MEM;
	while(sz--)
		*mem++ = 0;
	cur_pos = 0;
}

void 
backspace(void)
{
	if(!cur_pos)
		return;
	char *mem = VGA_MEM + (cur_pos -= 2);
	*mem++ = 0, *mem = 0;
}

void
prints(char *str)
{
	char *mem = VGA_MEM + cur_pos;
	while(*str)
	{
		*mem++ = *str, *mem++ = VGA_MODE;
		str++;
	}
	cur_pos = mem - VGA_MEM;
}

void 
printint(int c, int base)
{
	if(c < 0)
		putc('-'), c = -c;
	char *mem = VGA_MEM + cur_pos; 	//TODO
	int res = c > 0 ? c : -c;
	int mult = 1, tmp = res;

	do 
		mult *= base;
	while(tmp /= base);
	mult /= base;

	do
	{
		int tmp = res / mult + '0';
		putc(res / mult + ((tmp > '9') ? 'a' - 0xa : '0'));
		res %= mult;
	}
	while (mult /= base);
}

void printf(const char *fstr, ...)
{
	va_list p;
	va_start(p, fstr);
	int d;
	uint32_t x;
	char *s, c;

	for(char *str = fstr; *str; str++)
	{
		if(*str == '%')
		{
			switch(*++str)
			{
				case 'b':
					x = va_arg(p, uint32_t);
					printint(x, 2);
					break;
				case 'd':
					d = va_arg(p, int);
					printint(d, 10);
					break;
				case 'x':
					x = va_arg(p, uint32_t);
					printint(x, 16);
					break;
				case 's':
					s = va_arg(p, char *);
					prints(s);
					break;
				case 'c':
					c  = va_arg(p, char);
					putc(c);
					break;
				default:
					break;
			}
		}
		else if(*str == '\n')
			endline();
		else
			putc(*str);
	}
	va_end(p);
}

void
putc(char c)
{
	char *mem = VGA_MEM + cur_pos;
	*mem++ = c;
	*mem++ = VGA_MODE;
	cur_pos += 2;
}