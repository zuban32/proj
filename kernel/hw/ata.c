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
static uint32_t cur_buf_ind;
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
	add_local_dispatcher(ata_disp_func, ata_condition);
}

uint8_t is_bsy(void)
{
	return bsy;
}

uint32_t get_cur_ind(void)
{
	return cur_buf_ind;
}

void set_cur_ind(uint32_t ind)
{
	cur_buf_ind = ind;
}

uint16_t *get_ata_buffer(void)
{
	return ata_read_buffer;
}

void ata_request_readsector(int lba, uint8_t count, char slave)
{
	outb(PRIMARY_BASE_START + 6, 0xE0 | ((slave & 1) << 4) | ((lba >> 24) & 0x0F));
	outb(PRIMARY_BASE_START + 2, count);
	outb(PRIMARY_BASE_START + 3, lba & 0xFF);
	outb(PRIMARY_BASE_START + 4, (lba >> 8) & 0xFF);
	outb(PRIMARY_BASE_START + 5, (lba >> 16) & 0xFF);
	outb(PRIMARY_BASE_START + 7, 0x20);
}

void ata_complete_readsector(void)
{
	inb(PRIMARY_BASE_START + 7);
	bsy = 1;
	uint16_t *out = ata_read_buffer + cur_buf_ind * SECTOR_SIZE;
	for(int i = 0; i < SECTOR_SIZE; i++) {
		*(out + i) = inw(PRIMARY_BASE_START);
	}
	cur_buf_ind++;
	bsy = 0;
	pic_sendEOI(14);
}
