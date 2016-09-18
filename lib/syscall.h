#ifndef LIB_SYSCALL_H_
#define LIB_SYSCALL_H_

enum {
	SYS_PUTS,
};

// maybe should add par6(ebp) to be Linux compatible
inline int syscall(int num, int par1, int par2, int par3, int par4, int par5)
{
	int res;
	__asm __volatile(
			"int $0x80\n\t"
			:"=a"(res):"a"(num), "b"(par1), "c"(par2),"d"(par3),"S"(par4),"D"(par5)
			:"memory"
			);
	return res;
}

#endif /* LIB_SYSCALL_H_ */
