#ifndef INC_ABSTRACT_H_
#define INC_ABSTRACT_H_

#include <inc/common.h>

enum {
	MAX_TUNNELS_NUM 	= 128,
	MAX_EVENT_STRING 	= 32,
	EVENT_ERROR 		= -1
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
	SS_CHAR
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
	Event(int _type): type(_type) {}

	inline uint32_t get_msg() {
		return this->msg4;
	}

};

class Tunnel;

class Unit
{
	UnitType type;
	int subtype;
public:
	Unit(UnitType t, int st): type(t), subtype(st) {}
	virtual ~Unit() {}

	inline UnitType get_type()
	{
		return this->type;
	}
	inline int get_subtype()
	{
		return this->subtype;
	}

//	id is being used only when connecting to Driver.
//	can use any value otherwise
	Tunnel *connect_unit(int type, int id);

	virtual int init() = 0;
	virtual int handle(Event) = 0;
};

class API: public Unit
{
public:
	API(APIType t);
};

class Subsystem: public Unit
{
public:
	Subsystem(SubsystemType t);
};

class Driver: public Unit
{
public:
	Driver(DriverType t);

//	virtual ~Driver() {}
};

class Phys: public Unit
{
public:
	Phys(PhysType t);
};

class Tunnel
{
	Unit *a;
	Unit *b;
	int bytes_num;

public:
	Tunnel(): a(nullptr), b(nullptr), bytes_num(0) {}
	Tunnel(int _bytes_num): a(nullptr), b(nullptr), bytes_num(_bytes_num) {}
	virtual ~Tunnel() {}

	int init(Unit *_in, Unit *_out, int _bytes_num);

	virtual int transfer(Unit *me, Event e);
};

Tunnel *create_tunnel(Unit *in, Unit *out, int bytes_num);

#endif /* INC_ABSTRACT_H_ */
