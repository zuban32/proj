#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

#include <inc/elf.h>
#include <inc/isr.h>

enum {
	MAX_PROCESS_NUM = 256
};

enum {
	PROC_DEAD,
	PROC_EMPTY,
	PROC_RUNNING,
	PROC_READY
};

typedef struct {
	int pid;
	int status;
	void *code_start;
	Intframe iframe;
} Process;

Process *create_process(void);
Process *get_process_table(void);
Process *get_cur_process(void);
int load_process_code(Elf32_Ehdr *file, Process *proc);
void process_ret(Process *proc);

#endif /* INC_PROCESS_H_ */
