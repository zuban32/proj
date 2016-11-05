#include <syscall.h>
#include <common.h>

int get_pid(void)
{
	return syscall(SYS_GETPID, 0, 0, 0, 0, 0);
}

void process_exit(void)
{
	syscall(SYS_EXIT, 0, 0, 0, 0, 0);
}
