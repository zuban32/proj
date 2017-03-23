#include <registry.h>
#include <debug.h>

static Registry common_registry;

Registry *get_common_registry()
{
	return &common_registry;
}

int Registry::add_unit(Unit *u)
{
	int res = 0;
	if(this->cur_unit < MAX_UNITS) {
		dprintf("REG %x: adding unit (%d, %d) to pos %d\n", this, u->get_type(), u->get_subtype(), this->cur_unit);
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
	if(res) {
		dprintf("Registry %x: looking for unit (%d, %d): +\n", this, type, subtype);
	} else {
		dprintf("Registry %x: looking for unit (%d, %d): -\n", this, type, subtype);
	}
	return res;
}

int Registry::init()
{
	for(int i = 0; i < this->cur_unit; i++) {
		if(this->reg[i]->inited) {
			continue;
		}
		dprintf("REG: initing unit (%d, %d)\n", this->reg[i]->get_type(), this->reg[i]->get_subtype());
		int res = this->reg[i]->init();
		if(res) {
			dprintf("Registry: error initing unit (%d, %d)\n",
					this->reg[i]->get_type(), this->reg[i]->get_subtype());
			return -1;
		}
	}
	return 0;
}
