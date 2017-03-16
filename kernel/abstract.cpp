#include <inc/abstract.h>
#include <inc/registry.h>
#include <inc/string.h>
#include <inc/console.h>

static Tunnel tunnel_pool[MAX_TUNNELS_NUM];
static int cur_tun_pool_ind = 0;

Tunnel *create_tunnel(Unit *in, Unit *out)
{
	Tunnel *res = nullptr;
	if(cur_tun_pool_ind >= 0 && cur_tun_pool_ind < MAX_TUNNELS_NUM) {
		res = tunnel_pool + cur_tun_pool_ind++;
		res->init(in, out);
	}
	return res;
}

void Tunnel::init(Unit *_a, Unit *_b)
{
	this->a = _a;
	this->b = _b;
}

int Tunnel::transfer(Unit *me, Event e)
{
	kprintf("Tunnel transfer: [%d]\n", e.get_msg());
	if(me == this->a) {
		return this->b->handle(e);
	} else if(me == this->b) {
		return this->a->handle(e);
	} else {
		// error
		return -1;
	}
}

extern Registry common_registry;

Unit::Unit(int t, int st): type(t), subtype(st)
{
	common_registry.add_unit(this);
}

Tunnel *Unit::connect_to(int u_type, int u_subtype, int data)
{
	Tunnel *res = nullptr;
	Unit *to = common_registry.unit_lookup(u_type, u_subtype);
	if(to) {
		res = create_tunnel(this, to);
		to->connect_from(res, data);
	}
	return res;
}
