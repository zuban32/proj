#include <inc/abstract.h>
#include <inc/registry.h>
#include <inc/string.h>

static Tunnel tunnel_pool[MAX_TUNNELS_NUM];
static int cur_tun_pool_ind = 0;

Tunnel *create_tunnel(Unit *in, Unit *out, int bytes_num)
{
	Tunnel *res = nullptr;
	if(cur_tun_pool_ind >= 0 && cur_tun_pool_ind < MAX_TUNNELS_NUM) {
		res = tunnel_pool + cur_tun_pool_ind++;
		res->init(in, out, bytes_num);
	}
	return res;
}

int Tunnel::init(Unit *_in, Unit *_out, int _bytes_num)
{
	this->a = _in;
	this->b = _out;
	this->bytes_num = _bytes_num;

	return 0;
}

int Tunnel::transfer(Unit *me, Event e)
{
	if(me == this->a) {
		return b->handle(e);
	} else if(me == this->b) {
		return a->handle(e);
	} else {
		// error
		return -1;
	}
}

extern Registry common_registry;

API::API(APIType t): Unit(UNIT_API, t)
{
	common_registry.add_api(this);
}

Subsystem::Subsystem(SubsystemType t): Unit(UNIT_SUBSYSTEM, t)
{
	common_registry.add_subsystem(this);
}

Driver::Driver(DriverType t): Unit(UNIT_DRIVER, t)
{
	common_registry.add_driver(this);
}

Phys::Phys(PhysType t): Unit(UNIT_PHYS, t)
{
	common_registry.add_phys(this);
}

Tunnel *Unit::connect_unit(int utype, int id)
{
	Tunnel *res = nullptr;
	switch(this->type) {
	case UNIT_PHYS:
		// there's nothing below this level
		break;
	case UNIT_DRIVER:
		res = create_tunnel(common_registry.phys_lookup(utype), this, 0);
		break;
	case UNIT_SUBSYSTEM:
		res = create_tunnel(common_registry.driver_lookup(utype, id), this, 0);
		break;
	case UNIT_API:
		res = create_tunnel(common_registry.subsys_lookup(utype), this, 0);
		break;
	default:
		break;
	}
	return res;
}
