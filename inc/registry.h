#ifndef INC_REGISTRY_H_
#define INC_REGISTRY_H_

#include <inc/abstract.h>

enum {
	MAX_UNITS = 128,
};

class Registry
{
	Unit *reg[MAX_UNITS];
	int cur_unit = 0;

public:
	int add_unit(Unit *u);

	Unit *unit_lookup(unsigned type, unsigned sub_type);
};

#endif /* INC_REGISTRY_H_ */
