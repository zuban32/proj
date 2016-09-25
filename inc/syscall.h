#ifndef INC_SYSCALL_H_
#define INC_SYSCALL_H_

enum {
	SYS_PUTS,
	SYS_GETPID
};

#include <inc/process.h>

// maybe should add par6(ebp) to be Linux compatible
uint32_t syscall(Process *proc, uint32_t num, uint32_t par1, uint32_t par2, uint32_t par3, uint32_t par4, uint32_t par5);

#endif /* INC_SYSCALL_H_ */
