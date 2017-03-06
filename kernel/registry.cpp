#include <inc/registry.h>

Registry common_registry;

int Registry::add_phys(Phys *u)
{
	int res = 0;
	if(this->cur_phys < MAX_PHYS) {
		this->phys_reg[this->cur_phys++] = u;
	} else {
		res = -1;
	}
	return res;
}

int Registry::add_driver(Driver *u)
{
	int res = 0;
	if(this->cur_drv < MAX_DRIVER) {
		this->driver_reg[this->cur_drv++] = u;
	} else {
		res = -1;
	}

	return res;
}

int Registry::add_subsystem(Subsystem *u)
{
	int res = 0;
	if(this->cur_ss < MAX_SUBSYSTEM) {
		this->subsystem_reg[this->cur_ss++] = u;
	} else {
		res = -1;
	}
	return res;
}

int Registry::add_api(API *u)
{
	int res = 0;
	if(this->cur_api < MAX_API) {
		this->api_reg[this->cur_api++] = u;
	} else {
		res = -1;
	}
	return res;
}

API *Registry::api_lookup(int type)
{
	API *res = nullptr;
	for(int i = 0; i < this->cur_api; i++) {
		API *cur = this->api_reg[i];
		if(cur->get_subtype() == type) {
			res = cur;
			break;
		}
	}
	return res;
}

Subsystem *Registry::subsys_lookup(int type)
{
	Subsystem *res = nullptr;
	for(int i = 0; i < this->cur_ss; i++) {
		Subsystem *cur = this->subsystem_reg[i];
		if(cur->get_subtype() == type) {
			res = cur;
			break;
		}
	}
	return res;
}

Driver *Registry::driver_lookup(int type, int id)
{
	Driver *res = nullptr;
	for(int i = 0; i < this->cur_drv; i++) {
		Driver *cur = this->driver_reg[i];
		// add id check
		if(cur->get_subtype() == type) {
			res = cur;
			break;
		}
	}
	return res;
}

Phys *Registry::phys_lookup(int type)
{
	Phys *res = nullptr;
	for(int i = 0; i < this->cur_phys; i++) {
		Phys *cur = this->phys_reg[i];
		if(cur->get_subtype() == type) {
			res = cur;
			break;
		}
	}
	return res;
}
