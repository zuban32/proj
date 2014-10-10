//#include <inc/types.h>

#define VGA_MEM (char *)0xb8000
#define VGA_MODE 0x7

void print(char *);
void clear_screen(void);

int
main(void)
{
	clear_screen();
	print("Kernel loaded");
	return 0;
}

void
print(char *str)
{
	char *mem = VGA_MEM;
	while(*str)
	{
		*mem++ = *str, *mem++ = VGA_MODE;
		str++;
	}
}

void
clear_screen(void)
{
	int sz = 4000;
	char *mem = VGA_MEM;
	while(sz--)
		*mem++ = 0;
}