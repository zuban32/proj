#include <console.h>
#include <process.h>

void ProcessManager::sched(void)
{
	this->sched_enabled = true;
	Process *proc = this->head;
	if(this->cur_proc) {
		proc = this->cur_proc->get_next();
	}
	while(proc != this->cur_proc) {
		if(!proc) {
			proc = this->head;
		} else {
			if(proc->get_status() == PROC_READY) {
				this->exec_process(proc->get_pid());
			}
		}
		proc = proc->get_next();
	}
	kprintf("Panic: can't find process for scheduling\n");
	while(1);
}
