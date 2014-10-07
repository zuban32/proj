//#include <inc/types.h>

#define VGA_MEM 0xb8000
#define VGA_MODE 0x7

int
main(void)
{
	print("Kernel loaded\n");
	return 0;
}

void
print(char *str)
{
	char *mem = VGA_MEM;
	while(*str)
	{
		*mem = VGA_MODE, *(mem + 1) = *str;
		mem += 2;
		str++;
	}
}