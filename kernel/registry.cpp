#include <inc/registry.h>
#include <inc/console.h>

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

int Registry::init()
{
	for(int i = 0; i < this->cur_unit; i++) {
		int res = this->reg[i]->init();
		if(res) {
			kprintf("Registry: error initing unit (%d, %d)\n",
					this->reg[i]->get_type(), this->reg[i]->get_subtype());
			return -1;
		}
	}
	return 0;
}
