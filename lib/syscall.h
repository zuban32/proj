#ifndef LIB_SYSCALL_H_
#define LIB_SYSCALL_H_

enum {
	SYS_PUTC = 1,
};

#include <stdint.h>

// maybe should add par6(ebp) to be Linux compatible
uint32_t syscall(uint32_t num, uint32_t par1, uint32_t par2, uint32_t par3, uint32_t par4, uint32_t par5);

#endif /* LIB_SYSCALL_H_ */
