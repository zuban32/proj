#include "kbd.h"
#include "common.h"
#include "idt.h"
#include "pic.h"

#define VGA_MEM (char *)0xb8000
#define VGA_MODE 0x7
#define DISPLAY_WIDTH 80
#define DISPLAY_HEIGHT 25

static unsigned cur_pos = 0;

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
printint(char c, int base)
{
	char *mem = VGA_MEM + cur_pos; 	//TODO
	int res = c > 0 ? c : -c;
	if(res > 9)
		putc(res / base + 0x30);
	putc(res % base + 0x30);
	putc(20);
}

void
putc(char c)
{
	char *mem = VGA_MEM + cur_pos;
	*mem++ = c;
	*mem++ = VGA_MODE;
	cur_pos += 2;
}


int
kernel_main(void)
{
	clear_screen();
	
	load_idt();

	prints("Remap started");
	pic_remap(0x20, 0x28);
	prints("Remap ended");
	while(1);
	// prints("Kernel loaded");
	// int x = 5 / 0;
	// while(1);
	// kbd_init();
	// int i;
	// while(1)
	// 	printint(get_key());
	return 0;
}
