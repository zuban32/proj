#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include <stdint.h>

#include <abstract.h>

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

enum {
	MAX_CONSOLE_TUNS = 8
};

void kputc(char c, char move_bound);
void kprintf(const char *fstr, ...);

class Console: public Unit
{
	Tunnel *print_tun = nullptr;
	Tunnel *in_tuns[MAX_CONSOLE_TUNS] = {nullptr};
	int cur_in = 0;

public:
	Console(): Unit(UNIT_SUBSYSTEM, SS_CONSOLE) {
		deps[0][0] = UNIT_DRIVER;
		deps[0][1] = DRIVER_SERIAL;
		deps_num = 1;
	}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);
};

#endif

