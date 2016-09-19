#include <stdio.h>
#include <syscall.h>

int puts(const char *str)
{
	while(*str) {
		syscall(1, (uint32_t)*str, 0, 0, 0, 0);
		str++;
	}
	return 0;
}
