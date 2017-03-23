#ifndef SERIAL_H_INCLUDED
#define SERIAL_H_INCLUDED

#include <abstract.h>

#define COM1 0x3F8
#define SER_DIV 3

class SerialDriver: public Unit
{
	Tunnel *pic = nullptr;
	Tunnel *out = nullptr;
public:
	SerialDriver(): Unit(UNIT_DRIVER, DRIVER_SERIAL) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);
};

void write_serial(char);

#endif

