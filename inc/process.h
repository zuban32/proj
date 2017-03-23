#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

#include <elf.h>
#include <isr.h>
#include <abstract.h>

enum {
	MAX_PROCESS_NUM = 256,
	PROCESS_TUNS = 16
};

enum {
	PROC_FREE,
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
Process *create_kernel_process(void (*code)(void));
Process *get_process_table(void);
void free_process(Process *proc);

//void set_cur_process(Process *proc);
Process *get_cur_process(void);

int get_max_pid(void);
int load_process_code(Elf32_Ehdr *file, Process *proc);
void process_ret(Process *proc);

int sched_enabled(void);
void enable_sched(void);
void disable_sched(void);

void sched_yield(void);

class ProcessManager: public Unit
{
	Tunnel *to_mmu = nullptr;
	Tunnel *in_tuns[PROCESS_TUNS] = {nullptr};
public:
	ProcessManager(): Unit(UNIT_SUBSYSTEM, SS_PROCESS) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);
};

#endif /* INC_PROCESS_H_ */
