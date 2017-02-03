/*
 * controller.h
 *
 *  Created on: Oct 30, 2016
 *      Author: zuban32
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

#include <inc/isr.h>

#define MAX_DISPS 10

typedef int (*disp_condition)(int num);
typedef int (*dispatcher_func_t)(Intframe *iframe);

typedef struct
{
	dispatcher_func_t func;
	disp_condition cond;
} LocalController;

typedef struct
{
	LocalController dispatchers[MAX_DISPS];
	int dispatchers_num;
} GlobalController;

GlobalController global_controller;

void add_local_dispatcher(dispatcher_func_t func, disp_condition cond);
void global_dispatch(Intframe *iframe);

#endif /* INC_CONTROLLER_H_ */
