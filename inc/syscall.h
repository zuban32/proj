#ifndef INC_SYSCALL_H_
#define INC_SYSCALL_H_

#include <abstract.h>
#include <process.h>
#include <events/syscall.h>

enum {
	SYS_PUTS,
	SYS_GETPID,
	SYS_EXIT
};

class SyscallMgr: public Unit
{
	Tunnel *irq_tun = nullptr;
	Tunnel *proc_tun = nullptr;
public:
	SyscallMgr(): Unit(UNIT_API, API_SYSCALL) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);
};

#endif /* INC_SYSCALL_H_ */
