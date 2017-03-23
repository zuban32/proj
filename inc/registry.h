#ifndef INC_REGISTRY_H_
#define INC_REGISTRY_H_

#include <abstract.h>

enum {
	MAX_UNITS = 128,
};

// !!! _NO_ fields initialization: it's constructor must be called before any other,
//	BUT it can't be done in such a way, that's why everything is 0 at start by using _static_
class Registry
{
	Unit *reg[MAX_UNITS];
	int cur_unit;

public:
	int add_unit(Unit *u);

	Unit *unit_lookup(unsigned type, unsigned sub_type);
	int init();
};

Registry *get_common_registry();

#endif /* INC_REGISTRY_H_ */
