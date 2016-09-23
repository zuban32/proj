#include <inc/console.h>
#include <inc/process.h>

void sched_yield(void)
{
	if(sched_enabled()) {
		Process *cur_proc = get_cur_process();
		Process *proc_table = get_process_table();
		Process *cur = cur_proc? cur_proc + 1:proc_table;
		Process *proc_end = proc_table + get_max_pid();
		while(cur != cur_proc) {
			if(cur >= proc_end) {

				cur = proc_table;
			}
			kprintf("Sched: looking at proc %d\n", cur->id);
			if(cur->status == PROC_READY) {
				kprintf("Running this process\n");
				process_ret(cur);
			}
			cur++;
		}
		kprintf("Panic: can't find process for yielding\n");
	}
}
