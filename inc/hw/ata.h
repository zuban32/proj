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

	SECTOR_SIZE = 256,	// 256 word-size values
	READ_BUFFER_SEC_NUM = 16,
	READ_BUFFER_SIZE = READ_BUFFER_SEC_NUM * SECTOR_SIZE,

	MAX_ATA_DEVICES = 4
};

class ATADevice
{
	bool master = true;
	int ctrl_port = PRIMARY_CONTROL_PORT;
	int base_port = PRIMARY_BASE_START;
	int max_lba = 0;

	uint16_t read_buffer[READ_BUFFER_SIZE];
	uint8_t cur_buf_ind = 0;
	uint8_t bsy = 0;

public:
	ATADevice(bool _master, int _ctrl_port, int _base_port, int _max_lba):
		master(_master),
		ctrl_port(_ctrl_port),
		base_port(_base_port),
		max_lba(_max_lba)
	{}

	int read(int start_lba, int count, uint8_t *out);
};

class ATADriver: public Unit
{
public:
	Tunnel *irq_p_tun = nullptr;
	Tunnel *irq_s_tun = nullptr;
	Tunnel *in_tun = nullptr;

	ATADevice devs[MAX_ATA_DEVICES];

	ATADriver(): Unit(UNIT_DRIVER, DRIVER_ATA) {}

	int init();
	int connect_from(Tunnel *t, int data);
	int handle(Event e, void *ret);

	~ATADriver();
};

void ata_request_readsector(int lba, uint8_t count);
void ata_complete_readsector(void);

uint8_t is_bsy(void);
uint8_t get_cur_ind(void);
uint16_t *get_ata_buffer(void);

#endif
