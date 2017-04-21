#ifndef INC_HW_ATA_H_
#define INC_HW_ATA_H_

#include <abstract.h>
#include <util/port.h>

enum {
	PR_SEC_DIFF = 0x80,

	PRIMARY_BASE_START = 0x1F0,
	SECONDARY_BASE_START = PRIMARY_BASE_START - PR_SEC_DIFF,
	TOTAL_BASE_NUM = 8,

	PRIMARY_CONTROL_PORT = 0x3F6,
	SECONDARY_CONTROL_PORT = PRIMARY_CONTROL_PORT - PR_SEC_DIFF,

	SECTOR_SIZE = 512,	// 256 word-size values
	READ_BUFFER_SEC_NUM = 8,
	READ_BUFFER_SIZE = READ_BUFFER_SEC_NUM * SECTOR_SIZE,

	MAX_ATA_DEVICES = 4
};

struct ATAId
{
	int bus;
	int slave;

	ATAId(): bus(-1), slave(-1) {}
	ATAId(int _bus, int _slave): bus(_bus), slave(_slave) {}

	inline bool is_valid() {
		return bus >= 0 && bus < 2 && slave >= 0 && slave < 2;
	}

	inline int get_ind()
	{
		return (bus << 1) | slave;
	}
};

class ATADevice
{
	ATAId id;
	int ctrl_port = PRIMARY_CONTROL_PORT;
	int base_port = PRIMARY_BASE_START;
	int slave = 0;
	int max_lba = 0;

	uint16_t read_buffer[READ_BUFFER_SIZE];
	uint8_t cur_buf_ind = 0;
	uint8_t bsy = 0;

public:
	ATADevice() {}
	ATADevice(ATAId _id, int _ctrl_port, int _base_port):
		id(_id),
		ctrl_port(_ctrl_port),
		base_port(_base_port)
	{}

	inline ATAId get_id()
	{
		return this->id;
	}

	void set_max_lba(int max_lba);

	int read(int start_lba, int count, uint8_t *out);
	void handle_irq();
};

class ATADriver: public Unit
{
	Tunnel *irq_p_tun = nullptr;
	Tunnel *irq_s_tun = nullptr;
	Tunnel *in_tun = nullptr;

	ATADevice devs[MAX_ATA_DEVICES] = {
			ATADevice(ATAId(0, 0), PRIMARY_CONTROL_PORT, PRIMARY_BASE_START),
			ATADevice(ATAId(0, 1), PRIMARY_CONTROL_PORT, PRIMARY_BASE_START),
			ATADevice(ATAId(1, 0), SECONDARY_CONTROL_PORT, SECONDARY_BASE_START),
			ATADevice(ATAId(1, 1), SECONDARY_CONTROL_PORT, SECONDARY_BASE_START)
	};
	int device_num = 0;
	ATADevice *cur_dev = nullptr;

public:
	ATADriver(): Unit(UNIT_DRIVER, DRIVER_ATA) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

	inline int get_device_num()
	{
		return this->device_num;
	}

	inline ATADevice *get_device(int ind)
	{
		if(ind < 0 || ind >= this->device_num) {
			return nullptr;
		}
		return &this->devs[ind];
	}

	inline ATADevice *get_device(ATAId id)
	{
		return this->get_device(id.get_ind());
	}

	// bus: 0/1 - primary/secondary,
	// id:  0/1 - master/slave
	int device_select(ATAId id);
	int read(int start_lba, int count, uint8_t *out);

	~ATADriver() {}
};

#endif
