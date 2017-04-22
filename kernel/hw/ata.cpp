#include <hw/ata.h>
#include <hw/pic.h>
#include <console.h>
#include <util/string.h>
#include <debug.h>

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

static ATADriver ata_driver;

int ATADriver::init()
{
//	this->port_tun = this->connect_to(UNIT_PHYS, PHYS_PORT, 0);
	this->irq_p_tun = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 0xE);
	this->irq_s_tun = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 0xF);
	if(!this->irq_p_tun || !this->irq_s_tun) {
		return -1;
	}
	this->device_num = 2;
	this->devs[0].set_max_lba(INT32_MAX);
	this->devs[1].set_max_lba(INT32_MAX);
	this->device_select(ATAId(0, 1));
	return 0;
}

int ATADriver::connect_from(Tunnel *t, int data)
{
	if(!t) {
		return -1;
	}
	this->in_tun = t;
	return 0;
}

int ATADriver::handle(Event e, void *ret)
{
	dprintf("ATA driver irq\n");
	this->cur_dev->handle_irq();
//	Send EOI
	this->irq_p_tun->transfer(this, Event(1, 0xE), nullptr);
	return 0;
}

int ATADriver::device_select(ATAId id)
{
	int ind = (id.bus << 1) | id.slave;
	if(ind >= this->device_num) {
		return 1;
	}
	this->cur_dev = this->devs + ind;
	dprintf("Devoce %d %d selected\n", cur_dev->get_id().bus, cur_dev->get_id().slave);
	return 0;
}

int ATADriver::read(int start_lba, int count, uint8_t *out)
{
	if(!this->cur_dev) {
		return 0x32;
	}
	return this->cur_dev->read(start_lba, count, out);
}

int ATADevice::read(int start_lba, int count, uint8_t *out)
{
	if(start_lba < 0 || start_lba >= this->max_lba || count <= 0 || start_lba + count >= max_lba || !out) {
		return 1;
	}
	this->cur_buf_ind = 0;
	int prev_ind = this->cur_buf_ind;
	int out_off = 0;

	dprintf("ATA read: %d %d\n", start_lba, count);

	outb(this->base_port + 6, 0xE0 | ((this->id.slave & 1) << 4) | ((start_lba >> 24) & 0x0F));
	outb(this->base_port + 2, count);
	outb(this->base_port + 3, start_lba & 0xFF);
	outb(this->base_port + 4, (start_lba >> 8) & 0xFF);
	outb(this->base_port + 5, (start_lba >> 16) & 0xFF);
	outb(this->base_port + 7, 0x20);
	dprintf("Sent to ports\n");

	int read = 0;
	while(this->bsy || read < count) {
		if(this->cur_buf_ind > prev_ind) {
			dprintf("Read sector\n");
			prev_ind = this->cur_buf_ind;
			kmemcpy((char *)out + out_off, (char *)this->read_buffer, SECTOR_SIZE);
			out_off += SECTOR_SIZE;
			this->cur_buf_ind = 0;
			read++;
		}
	}

	return 0;
}

void ATADevice::handle_irq()
{
	dprintf("ATA Irq\n");
	inb(this->base_port + 7);
	this->bsy = 1;
	uint16_t *out = this->read_buffer + this->cur_buf_ind * SECTOR_SIZE / 2;
	for(unsigned i = 0; i < SECTOR_SIZE / 2; i++) {
		*(out + i) = inw(this->base_port);
	}
	this->cur_buf_ind++;
	this->bsy = 0;
}

