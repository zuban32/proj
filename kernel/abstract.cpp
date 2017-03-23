#include <abstract.h>
#include <registry.h>
#include <string.h>
#include <console.h>

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

int Tunnel::transfer(Unit *me, Event e, void *ret)
{
	if(me == this->a) {
		return this->b->handle(e, ret);
	} else if(me == this->b) {
		return this->a->handle(e, ret);
	} else {
		// error
		return -1;
	}
}

Unit::Unit(int t, int st)
{
	this->type = t;
	this->subtype = st;
	get_common_registry()->add_unit(this);
}

Tunnel *Unit::connect_to(int u_type, int u_subtype, int data)
{
	Tunnel *res = nullptr;
	Unit *to = get_common_registry()->unit_lookup(u_type, u_subtype);
	if(to) {
		res = create_tunnel(this, to);
		to->connect_from(res, data);
	}
	return res;
}
