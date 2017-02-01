#ifndef INC_ABSTRACT_H_
#define INC_ABSTRACT_H_

#include <inc/common.h>

enum {
	MAX_TUNNELS_NUM = 128
};

class Unit;


// every socket has to implement init(...) method,
// example of reason is ISR_Socket that must know its number
class Socket
{
protected:
	uint8_t *buf;
	Unit *u;

public:
	Socket(): buf(nullptr), u(nullptr) {}
	Socket(Unit *_u): buf(nullptr), u(_u) {}

//	virtual int init();
	virtual int send() = 0;
	virtual int recv() = 0;

	char *get_data(void) {
		return (char *)this->buf;
	}

	virtual ~Socket() {}
};

class Unit
{
public:
	virtual int handle(Socket *) = 0;

	virtual ~Unit() {}
};

class Tunnel
{
	Socket *in;
	Socket *out;
	int bytes_num;

public:
	Tunnel(): in(nullptr), out(nullptr), bytes_num(0) {}
	Tunnel(int _bytes_num): in(nullptr), out(nullptr), bytes_num(_bytes_num) {}

	int init(Socket *_in, Socket *_out, int _bytes_num);

	virtual int transfer();

	virtual ~Tunnel() {}
};

Tunnel *create_tunnel(Socket *in, Socket *out, int bytes_num);

#endif /* INC_ABSTRACT_H_ */
