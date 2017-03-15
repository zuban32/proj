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

ATADriver ata_driver;

ATADriver::~ATADriver()
{
	kprintf("Ata destructor\n");
}

int ATADriver::init()
{
	this->port_tun = this->connect_to(UNIT_PHYS, PHYS_PORT);
	this->irq_tun = this->connect_to(UNIT_PHYS, PHYS_IRQ);
	if(!this->port_tun || !this->irq_tun) {
		return -1;
	}
	return 0;
}

int ATADriver::connect_from(Tunnel *t)
{
	if(!t) {
		return -1;
	}
	this->in_tun = t;
	return 0;
}

int ATADriver::handle(Event e)
{
	return 0;
}

void init_ata(void)
{
//	add_local_dispatcher(&ata_driver);
	kprintf("ATA dbg = %x\n", ata_driver.dbg);
	kprintf("ATA inited\n");

}

uint8_t is_bsy(void)
{
	return ata_driver.bsy;
}

uint8_t get_cur_ind(void)
{
	return ata_driver.cur_buf_ind;
}

uint16_t *get_ata_buffer(void)
{
	return ata_driver.ata_read_buffer;
}

void ata_request_readsector(int lba, uint8_t count)
{
	outb(PRIMARY_BASE_START + 6, 0xE0 | ((lba >> 24) & 0x0F));	// choose master
	outb(PRIMARY_BASE_START + 2, count);
	outb(PRIMARY_BASE_START + 3, lba & 0xFF);
	outb(PRIMARY_BASE_START + 4, (lba >> 8) & 0xFF);
	outb(PRIMARY_BASE_START + 5, (lba >> 16) & 0xFF);
	outb(PRIMARY_BASE_START + 7, 0x20);
	kprintf("READ SECTOR request\n");
}

void ata_complete_readsector(void)
{
	inb(PRIMARY_BASE_START + 7);
	ata_driver.bsy = 1;
	uint16_t *out = ata_driver.ata_read_buffer + ata_driver.cur_buf_ind * SECTOR_SIZE;
	for(int i = 0; i < SECTOR_SIZE; i++) {
		*(out + i) = inw(PRIMARY_BASE_START);
	}
	ata_driver.cur_buf_ind++;
	ata_driver.bsy = 0;
	pic_sendEOI(14);
}
