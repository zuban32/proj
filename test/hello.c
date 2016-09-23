#include <stdio.h>
#include <common.h>

int main(void)
{
//	puts("Hello, World!\n", 14);
//	puts("print again\n", 12);
	char num = '0' + get_pid();
	puts("Process #", 9);
	puts(&num, 1);
	puts("\n", 1);
	while(1);	// just to make sched work forever
	return 0;
}
