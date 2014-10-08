//#include <inc/types.h>

#define VGA_MEM 0xb8000
#define VGA_MODE 0x7

int
main(void)
{
	// char *mem = VGA_MEM;
	// *mem = 'X';
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
		*mem = *str, *(mem + 1) = VGA_MODE;
		mem += 2;
		str++;
	}
}

void
clear_screen(void)
{
	int sz = 4000;
	char *mem = VGA_MEM;
	while(sz--)
	{
		*mem++ = 0;
	}
}