#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

#include <inc/elf.h>

enum {
	MAX_PROCESS_NUM = 32
};

typedef struct {
	int pid;
	void *code_start;
} Process;

Process *create_process(void);
Process *get_process(int pid);
int load_process_code(Elf32_Ehdr *file, Process *proc);

#endif /* INC_PROCESS_H_ */
