#include "console.h"

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
	cur_pos = (DISPLAY_WIDTH << 1) * (cur_pos / (DISPLAY_WIDTH << 1) + 1);
}

void 
printint(int c, int base)
{
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

	cur_pos = (DISPLAY_WIDTH << 1) * (cur_pos / (DISPLAY_WIDTH << 1) + 1);
}

void
putc(char c)
{
	char *mem = VGA_MEM + cur_pos;
	*mem++ = c;
	*mem++ = VGA_MODE;
	cur_pos += 2;
}