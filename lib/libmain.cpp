#include <common.h>

extern void main(void);

extern "C" void usermain(void)
{
	main();
	process_exit();
}
