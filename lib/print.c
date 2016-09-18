#include "stdio.h"
#include "syscall.h"

int puts(const char *str)
{
	return syscall(SYS_PUTS, str, 0, 0, 0, 0);
}
