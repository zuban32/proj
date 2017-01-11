#ifndef INC_ABSTRACT_H_
#define INC_ABSTRACT_H_

#include <inc/isr.h>

enum ESys {
	MAX_DRIVERS = 32,
	MAX_DRIVER_NAME = 64
};

typedef int (*init_func_t)(void);
typedef int (*irq_condition)(Intframe *iframe);
typedef int (*irq_func_t)(Intframe *iframe);

typedef struct
{
	init_func_t init;
	irq_func_t handler;
	irq_condition cond;
	char desc[MAX_DRIVER_NAME];
} Driver;

typedef struct {
	Driver *drivers[MAX_DRIVERS];
	int drivers_num;
} System;

#endif /* INC_ABSTRACT_H_ */
