#include <inc/controller.h>
#include <inc/console.h>

extern GlobalController global_controller;

void global_dispatch(Intframe *iframe)
{
	int found = 0;
	for(int i = 0; i < global_controller.dispatchers_num; i++) {
		if(global_controller.dispatchers[i].cond(iframe->intno)) {
			found = 1;
			global_controller.dispatchers[i].func(iframe);
			break;
		}
	}

	if(!found) {
		kprintf("Panic: dispatcher for int %d not found\n", iframe->intno);
		while(1);
	}
}

void add_local_dispatcher(dispatcher_func_t func, disp_condition cond)
{
	global_controller.dispatchers[global_controller.dispatchers_num].func = func;
	global_controller.dispatchers[global_controller.dispatchers_num++].cond = cond;
}
