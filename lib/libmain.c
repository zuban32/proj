#include <common.h>

extern void main(void);

void usermain(void)
{
	main();
	process_exit();
}
