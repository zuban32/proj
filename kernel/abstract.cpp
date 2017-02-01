#include <inc/abstract.h>
#include <inc/string.h>

static Tunnel tunnel_pool[MAX_TUNNELS_NUM];
static int cur_tun_pool_ind = 0;

Tunnel *create_tunnel(Socket *in, Socket *out, int bytes_num)
{
	Tunnel *res = nullptr;
	if(cur_tun_pool_ind >= 0 && cur_tun_pool_ind < MAX_TUNNELS_NUM) {
		res = tunnel_pool + cur_tun_pool_ind++;
		res->init(in, out, bytes_num);
	}
	return res;
}

int Tunnel::init(Socket *_in, Socket *_out, int _bytes_num)
{
	this->in = _in;
	this->out = _out;
	this->bytes_num = _bytes_num;

	return 0;
}

int Tunnel::transfer()
{
	in->send();
	kmemcpy((char*)out->get_data(), in->get_data(), this->bytes_num);
	out->recv();
	return 0;
}
