#include <inc/controller.h>
#include <inc/console.h>

GlobalController global_controller;

void global_dispatch(Intframe *iframe)
{
	int found = 0;
	for(int i = 0; i < global_controller.dispatchers_num; i++) {
		kprintf("Call local dispatcher cond\n");
		if(global_controller.dispatchers[i]->cond(iframe->intno)) {
			found = 1;
			kprintf("Call local dispatcher\n");
			global_controller.dispatchers[i]->func(iframe);
			break;
		}
	}

	if(!found) {
		kprintf("Panic: dispatcher for int %d not found\n", iframe->intno);
		while(1);
	}
}

//void add_local_dispatcher(dispatcher_func_t func, disp_condition cond)
//{
//	global_controller.dispatchers[global_controller.dispatchers_num].func = func;
//	global_controller.dispatchers[global_controller.dispatchers_num++].cond = cond;
//}

void add_local_dispatcher(LocalController *c)
{
	global_controller.dispatchers[global_controller.dispatchers_num++] = c;
}
//
//bool LocalController::cond(int num)
//{
//	kprintf("PANIC: dummy cond function called!\n");
//	return true;
//}
//
//int LocalController::func(Intframe *iframe)
//{
//	kprintf("PANIC: dummy func function called!\n");
//	return -1;
//}
