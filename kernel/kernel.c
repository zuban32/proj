//#include <inc/types.h>

#define VGA_MEM (char *)0xb8000
#define VGA_MODE 0x7
#define DISPLAY_WIDTH 80
#define DISPLAY_HEIGHT 25

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned uint32_t;
typedef unsigned uintptr_t;

enum keys
{
	KEY_A = 0x1e,
	KEY_B = 0x30,
	KEY_C = 0x2e,
	KEY_D = 0x20,
	KEY_E = 0x12,
	KEY_F = 0x21,
	KEY_G = 0x22,
	KEY_H = 0x23,
	KEY_I = 0x17,
	KEY_J = 0x24,
	KEY_K = 0x25,
	KEY_L = 0x26,
	KEY_M = 0x32,
	KEY_N = 0x31,
	KEY_O = 0x18,
	KEY_P = 0x19,
	KEY_Q = 0x10,
	KEY_R = 0x13,
	KEY_S = 0x1f,
	KEY_T = 0x14,
	KEY_U = 0x16,
	KEY_V = 0x2f,
	KEY_W = 0x11,
	KEY_X = 0x2d,
	KEY_Y = 0x15,
	KEY_Z = 0x2c,
	KEY_0 = 0xb,
	KEY_1 = 0x2,
	KEY_2 = 0x3,
	KEY_3 = 0x4,
	KEY_4 = 0x5,
	KEY_5 = 0x6,
	KEY_6 = 0x7,
	KEY_7 = 0x8,
	KEY_8 = 0x9,
	KEY_9 = 0xa
};

static unsigned cur_pos = 0;

uint8_t
inb(int port)
{
	uint8_t data;
	__asm __volatile("inb %w1,%0" : "=a" (data) : "d" (port));
	return data;
}

void
outb(int port, uint8_t data)
{
	__asm __volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

void 
kbd_ack(void)
{
	prints("ACKing ");
	while(inb(0x64) != 0xfa);
}

void
kbd_init(void)
{
	prints("init ");
	//outb(0x64, 0xf4);
	//kbd_ack();
}

char 
get_key()
{
	prints("getting key ");
	return inb(0x60);
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
printint(char c)
{
	char *mem = VGA_MEM + cur_pos;
	int res = c > 0 ? c : -c;
	putc(res / 16 + 0x30);
	putc(res % 16 + 0x30);
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
	prints("Kernel loaded ");
	kbd_init();
	int i;
	while(1)
	printint(get_key());
	return 0;
}