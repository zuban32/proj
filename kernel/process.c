#include <inc/common.h>
#include <inc/process.h>
#include <inc/console.h>

static int cur_max_pid = 0;
static Process process_table[MAX_PROCESS_NUM];

Process *create_process(void)
{
	if(cur_max_pid + 1 == MAX_PROCESS_NUM) {
		return NULL;
	}
	return &process_table[cur_max_pid++];
}

Process *get_process(int pid)
{
	if(pid < 0 || pid > MAX_PROCESS_NUM) {
		return NULL;
	}
	return &process_table[pid];
}

int load_process_code(Elf32_Ehdr *file, Process *proc)
{
	if(!is_elf(file)) {
		kprintf("Error loading ELF: it's not ELF\n");
		return 1;
	}
	return 0;
}
