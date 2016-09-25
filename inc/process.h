#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

#include <inc/elf.h>
#include <inc/isr.h>

enum {
	MAX_PROCESS_NUM = 256
};

enum {
	PROC_EMPTY,
	PROC_TAKEN,
	PROC_READY,
	PROC_RUNNING,
	PROC_DEAD
};

typedef struct Process Process;

typedef struct Process {
	int id;
	int status;
	void *code_start;
	Intframe iframe;
	Process *next;
	Process *prev;
} Process;

Process *create_process(Elf32_Ehdr *file);
Process *get_process_table(void);

void set_cur_process(Process *proc);
Process *get_cur_process(void);

int get_max_pid(void);
int load_process_code(Elf32_Ehdr *file, Process *proc);
void process_ret(Process *proc);

int sched_enabled(void);
void enable_sched(void);

void sched_yield(void);

#endif /* INC_PROCESS_H_ */
