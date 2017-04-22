#ifndef INC_PROCESS_H_
#define INC_PROCESS_H_

#include <elf.h>
#include <isr.h>
#include <abstract.h>
#include <fio.h>

enum {
	MAX_PROCESS_NUM = 256,
	PROCESS_TUNS = 16
};

enum ProcessStatus {
	PROC_FREE,
	PROC_TAKEN,
	PROC_READY,
	PROC_RUNNING,
	PROC_DEAD
};

class Process;

class Process {
	static int global_pid;
	int id = global_pid++;
	ProcessStatus status = PROC_FREE;
	void *code_start = nullptr;
	Intframe iframe;
	Process *next = nullptr;
	Process *prev = nullptr;

public:
	inline int get_pid()
	{
		return this->id;
	}

	inline ProcessStatus get_status()
	{
		return this->status;
	}

	inline void set_status(ProcessStatus s)
	{
		this->status = s;
	}

	inline void set_prev(Process *p)
	{
		this->prev = p;
	}

	inline void set_next(Process *p)
	{
		this->next = p;
	}

	inline Process *get_prev()
	{
		return this->prev;
	}

	inline Process *get_next()
	{
		return this->next;
	}

	inline Intframe *get_iframe()
	{
		return &this->iframe;
	}

	int finalize(uint32_t start, bool user);
	// start is optional - can be NULL
	int load_code(File *f, uint32_t *start);
	void exec();
	void free();

	void handle_irq(Intframe *i);
};

class ProcessManager: public Unit
{
	Tunnel *to_mmu = nullptr;
	Tunnel *to_pic = nullptr;
	Tunnel *in_tuns[PROCESS_TUNS] = {nullptr};

	Process *cur_proc = nullptr;
	Process process_table[MAX_PROCESS_NUM];

	Process *free_head = process_table + 1;
	Process *head = process_table;

	bool sched_enabled = false;

public:
	ProcessManager(): Unit(UNIT_SUBSYSTEM, SS_PROCESS) {}

	inline Process *get_cur_process()
	{
		return this->cur_proc;
	}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

	void handle_irq(Intframe *i);

	int create_process(File *f);
	int exec_process(int pid);
	void free_process(int pid);

	void sched();
};

int create_process(File *f);
void exec_process(int pid);
void free_process(int pid);
void sched_start();

#endif /* INC_PROCESS_H_ */
