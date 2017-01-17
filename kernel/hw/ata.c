#include <inc/ata.h>
#include <inc/console.h>
#include <inc/pic.h>
#include <inc/controller.h>

//static int ata_identify(int base)
//{
//	outb(base + 0x200 + 6, 2);
//	outb(base + 6, 0xA0);
//	outb(base + 2, 0);
//	outb(base + 3, 0);
//	outb(base + 4, 0);
//	outb(base + 5, 0);
//	outb(base + 7, 0xEC);
//
//	uint8_t status = 0;
//
//	status = inb(base + 7);
//	status = inb(base + 7);
//	status = inb(base + 7);
//	status = inb(base + 7);
//	status = inb(base + 7);
//
//	int i = 0;
//	for(i = 0; i < 8; i++) {
//		kprintf("[%d] = %d\n", i, (status & (1 << i)) >> i);
//	}
//
//	while((status & 1) || ((status & (1 << 7)) >> 7))
//		status = inb(base + 7);
//	for(i = 0; i < 8; i++) {
//		kprintf("[%d] = %d\n", i, (status & (1 << i)) >> i);
//	}
//	return 0;
//}

static uint16_t ata_read_buffer[READ_BUFFER_SIZE];
static uint8_t cur_buf_ind;
static uint8_t bsy;

int ata_condition(int num)
{
	return num == ISR_ATA;
}

int ata_disp_func(Intframe *iframe)
{
	ata_complete_readsector();
	return 0;
}

void init_ata(void)
{
<<<<<<< HEAD:kernel/hw/ata.c
	add_local_dispatcher(ata_disp_func, ata_condition);
=======
	add_local_dispatcher(&ata_driver);
	kprintf("ATA dbg = %x\n", ATADriver::dbg);
//	add_local_dispatcher(ata_disp_func, ata_condition);
>>>>>>> 7d191ed... Secondary bootloader works, but C++ virtual functions works only with static classes fields:kernel/hw/ata.cpp
}

uint8_t is_bsy(void)
{
<<<<<<< HEAD:kernel/hw/ata.c
	return bsy;
=======
	return ATADriver::bsy;
>>>>>>> 7d191ed... Secondary bootloader works, but C++ virtual functions works only with static classes fields:kernel/hw/ata.cpp
}

uint8_t get_cur_ind(void)
{
<<<<<<< HEAD:kernel/hw/ata.c
	return cur_buf_ind;
=======
	return ATADriver::cur_buf_ind;
>>>>>>> 7d191ed... Secondary bootloader works, but C++ virtual functions works only with static classes fields:kernel/hw/ata.cpp
}

uint16_t *get_ata_buffer(void)
{
<<<<<<< HEAD:kernel/hw/ata.c
	return ata_read_buffer;
=======
	return ATADriver::ata_read_buffer;
>>>>>>> 7d191ed... Secondary bootloader works, but C++ virtual functions works only with static classes fields:kernel/hw/ata.cpp
}

void ata_request_readsector(int lba, uint8_t count)
{
	outb(PRIMARY_BASE_START + 6, 0xE0 | ((lba >> 24) & 0x0F));	// choose master
	outb(PRIMARY_BASE_START + 2, count);
	outb(PRIMARY_BASE_START + 3, lba & 0xFF);
	outb(PRIMARY_BASE_START + 4, (lba >> 8) & 0xFF);
	outb(PRIMARY_BASE_START + 5, (lba >> 16) & 0xFF);
	outb(PRIMARY_BASE_START + 7, 0x20);
}

void ata_complete_readsector(void)
{
	inb(PRIMARY_BASE_START + 7);
<<<<<<< HEAD:kernel/hw/ata.c
	bsy = 1;
	uint16_t *out = ata_read_buffer + cur_buf_ind * SECTOR_SIZE;
	for(int i = 0; i < SECTOR_SIZE; i++) {
		*(out + i) = inw(PRIMARY_BASE_START);
	}
	cur_buf_ind++;
	bsy = 0;
=======
	ATADriver::bsy = 1;
	uint16_t *out = ATADriver::ata_read_buffer + ATADriver::cur_buf_ind * SECTOR_SIZE;
	for(int i = 0; i < SECTOR_SIZE; i++) {
		*(out + i) = inw(PRIMARY_BASE_START);
	}
	ATADriver::cur_buf_ind++;
	ATADriver::bsy = 0;
>>>>>>> 7d191ed... Secondary bootloader works, but C++ virtual functions works only with static classes fields:kernel/hw/ata.cpp
	pic_sendEOI(14);
}
