#ifndef INC_ATA_H_
#define INC_ATA_H_

#include <inc/common.h>

enum {
	PR_SEC_DIFF = 0x80,

	PRIMARY_BASE_START = 0x1F0,
	SECONDARY_BASE_START = PRIMARY_BASE_START - PR_SEC_DIFF,
	TOTAL_BASE_NUM = 8,

	PRIMARY_CONTROL_PORT = 0x3F6,
	SECONDARY_CONTROL_PORT = PRIMARY_CONTROL_PORT - PR_SEC_DIFF,

	SECTOR_SIZE = 256,	// 256 word-size values
	READ_BUFFER_SEC_NUM = 16,
	READ_BUFFER_SIZE = READ_BUFFER_SEC_NUM * SECTOR_SIZE
};

void ata_request_readsector(int lba, int count);
void ata_complete_readsector(void);

uint16_t ata_read_buffer[READ_BUFFER_SIZE];
uint8_t cur_buf_ind;
uint8_t is_bsy;

#endif
