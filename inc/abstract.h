#ifndef INC_ABSTRACT_H_
#define INC_ABSTRACT_H_

#include <util/port.h>
#include <registry.h>

enum {
	MAX_TUNNELS_NUM 	= 128,
	MAX_EVENT_STRING 	= 32,
	EVENT_ERROR 		= -1,
	MAX_DEPS			= 8
};

enum UnitType
{
	UNIT_PHYS,
	UNIT_DRIVER,
	UNIT_SUBSYSTEM,
	UNIT_API
};

enum APIType
{
	API_SYSCALL
};

enum SubsystemType
{
	SS_HEAP,
	SS_PROCESS,
	SS_GRAPHICS,
	SS_FILE,
	SS_CONSOLE
};

enum DriverType
{
	DRIVER_MMU,
	DRIVER_SERIAL,
	DRIVER_PIC,
	DRIVER_KBD,
	DRIVER_ATA,
	DRIVER_VESA,
	DRIVER_FAT32
};

enum PhysType
{
	PHYS_IRQ,
	PHYS_PORT
};

class Event
{
	int type;
	union {
		uint8_t msg1[4];
		uint16_t msg2[2];
		uint32_t msg4;
	};

public:
	Event(): type(EVENT_ERROR) {}
	Event(int _type): type(_type), msg4(0) {}
	Event(int _type, uint32_t _msg4): type(_type), msg4(_msg4) {}

	inline int get_type() {
		return this->type;
	}

	inline uint32_t get_msg() {
		return this->msg4;
	}

};

class Tunnel;
class Registry;

class Unit
{
	friend class Registry;
	unsigned type;
	unsigned subtype;


protected:
	int deps[MAX_DEPS][2];
	int deps_num = 0;
	Registry *reg;

public:
	Unit(int t, int st);
	virtual ~Unit() {}

	bool inited = false;

	inline unsigned get_type()
	{
		return this->type;
	}
	inline unsigned get_subtype()
	{
		return this->subtype;
	}

	inline void set_inited()
	{
		this->inited = true;
	}

	Tunnel *connect_to(int type, int subtype, int data);
	virtual int connect_from(Tunnel *t, int data) = 0;

	virtual int init() = 0;
	virtual int handle(Event e, void *ret) = 0;
};

class Tunnel
{
	Unit *a;
	Unit *b;

public:
	Tunnel(): a(nullptr), b(nullptr) {}
	virtual ~Tunnel() {}

	void init(Unit *a, Unit *b);
	virtual int transfer(Unit *me, Event e, void *ret);
};

Tunnel *create_tunnel(Unit *in, Unit *out);

#endif /* INC_ABSTRACT_H_ */
