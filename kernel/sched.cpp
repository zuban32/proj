#include <console.h>
#include <process.h>

void sched_yield(void)
{
	if(sched_enabled()) {
		Process *cur_proc = get_cur_process();
		Process *proc_table = get_process_table();
		Process *cur = cur_proc? cur_proc + 1:proc_table;
		Process *proc_end = proc_table + get_max_pid();
		Process *ker_proc = nullptr;
		while(cur != cur_proc) {
			if(cur >= proc_end) {
				cur = proc_table;
			}
			if(cur->status == PROC_READY) {
				if(cur->iframe.ret_cs == 0x8 && !ker_proc) {
					ker_proc = cur;
				} else {
					process_ret(cur);
				}
			}
			cur++;
		}
		if(ker_proc) {
			process_ret(ker_proc);
		}
		kprintf("Panic: can't find process for yielding\n");
		while(1);
	}
}
