#include <stdio.h>
#include <syscall.h>

int puts(const char *str, int len)
{
	syscall(SYS_PUTS, (uint32_t)str, len, 0, 0, 0);
	return 0;
}
