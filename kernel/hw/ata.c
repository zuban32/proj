#include <inc/ata.h>
#include <inc/console.h>
#include <inc/pic.h>

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

//static int ata_init(void);
static int ata_irq_condition(Intframe *iframe);
static int ata_irq_handler(Intframe *iframe);

Driver ata_driver = {
		ata_init,
		ata_irq_condition,
		ata_irq_handler,
		"ATA core driver"
};

static ATA_State state = {
		.base = &ata_driver
};

static int ata_irq_condition(Intframe *iframe)
{
	return iframe->intno == ISR_ATA;
}

static int ata_irq_handler(Intframe *iframe)
{
	ata_complete_readsector();
	return 0;
}

int ata_init(void)
{
	state.bsy = 0;
	state.cur_buf_ind = 0;

	return 0;
}


uint8_t is_bsy(void)
{
	return state.bsy;
}

uint8_t get_cur_ind(void)
{
	return state.cur_buf_ind;
}

uint16_t *get_ata_buffer(void)
{
	return state.ata_read_buffer;
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
	state.bsy = 1;
	uint16_t *out = state.ata_read_buffer + state.cur_buf_ind * SECTOR_SIZE;
	for(int i = 0; i < SECTOR_SIZE; i++) {
		*(out + i) = inw(PRIMARY_BASE_START);
	}
	state.cur_buf_ind++;
	state.bsy = 0;
	pic_sendEOI(14);
}
