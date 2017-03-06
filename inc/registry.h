#ifndef INC_REGISTRY_H_
#define INC_REGISTRY_H_

#include <inc/abstract.h>

enum {
	MAX_API 		= 4,
	MAX_SUBSYSTEM 	= 16,
	MAX_DRIVER 		= 32,
	MAX_PHYS 		= 2	// irq and port
};

class Registry
{
	API *		api_reg[MAX_API];
	Subsystem *	subsystem_reg[MAX_SUBSYSTEM];
	Driver *	driver_reg[MAX_DRIVER];
	Phys *		phys_reg[MAX_PHYS];
	int cur_api = 0;
	int cur_ss = 0;
	int cur_drv = 0;
	int cur_phys = 0;

public:
	int add_phys(Phys *u);
	int add_driver(Driver *u);
	int add_subsystem(Subsystem *u);
	int add_api(API *u);

	API *		api_lookup(int type);
	Subsystem *	subsys_lookup(int type);
	Driver *	driver_lookup(int type, int id);
	Phys *		phys_lookup(int type);
};

#endif /* INC_REGISTRY_H_ */
