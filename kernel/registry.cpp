#include <inc/registry.h>

Registry common_registry;

int Registry::add_unit(Unit *u)
{
	int res = 0;
	if(this->cur_unit < MAX_UNITS) {
		this->reg[this->cur_unit++] = u;
	} else {
		res = -1;
	}
	return res;
}

Unit *Registry::unit_lookup(unsigned type, unsigned subtype)
{
	Unit *res = nullptr;
	for(int i = 0; i < this->cur_unit; i++) {
		Unit *cur = this->reg[i];
		if(cur->get_type() == type && cur->get_subtype() == subtype) {
			res = cur;
			break;
		}
	}
	return res;
}
