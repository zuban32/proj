#include <syscall.h>

uint32_t syscall(uint32_t num, uint32_t par1, uint32_t par2, uint32_t par3, uint32_t par4, uint32_t par5)
{
	int res;
	__asm __volatile(
			"int $0x80\n\t"
			:"=a"(res):"a"(num), "b"(par1), "c"(par2),"d"(par3),"S"(par4),"D"(par5)
			:"memory"
			);
	return res;
}
