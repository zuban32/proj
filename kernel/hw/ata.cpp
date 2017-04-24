#include <hw/ata.h>
#include <hw/pic.h>
#include <console.h>
#include <util/string.h>
#include <debug.h>

//static void dump_status(char byte)
//{
//	dprintf("-------------------\n");
//	dprintf("Raw val = %x\n", byte);
//	dprintf("Err = %x, DRQ = %x, SRV = %x, ", byte & 7, (byte & 8) >> 3, (byte & 16) >> 4);
//	dprintf("DF = %x, RDY = %x, BSY = %x\n", (byte & 32) >> 5, (byte & 64) >> 6, (byte & 128) >> 7);
//}

bool ATADevice::identify()
{
	outb(this->base_port + 6, 0xA0 | (this->id.slave << 4));
	outb(this->base_port + 2, 0);
	outb(this->base_port + 3, 0);
	outb(this->base_port + 4, 0);
	outb(this->base_port + 5, 0);
	outb(this->base_port + 7, 0xEC);

	int res = inb(this->base_port + 7);
	if(!res) {
		return false;
	}
	while(inb(this->base_port + 7) & 0x80);
	if(inb(this->base_port + 4) || inb(this->base_port + 5)) {
		return false;
	}
	int status;
	do {
		status = inb(this->base_port + 7);
	} while(!((status | 8) || (status | 1)));
	if(!(status & 1)) {
		uint16_t *out = this->read_buffer + this->cur_buf_ind * SECTOR_SIZE / 2;
		for(unsigned i = 0; i < SECTOR_SIZE / 2; i++) {
			*(out + i) = inw(this->base_port);
		}
		uint32_t *dev_lba = (uint32_t *)(this->read_buffer + this->cur_buf_ind + 60);
		this->max_lba = *dev_lba;
		this->cur_buf_ind++;
	} else {
		return false;
	}

	this->valid = true;
	return true;
}

static ATADriver ata_driver;

int ATADriver::init()
{
//	this->port_tun = this->connect_to(UNIT_PHYS, PHYS_PORT, 0);
	this->irq_p_tun = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 0xE);
	this->irq_s_tun = this->connect_to(UNIT_DRIVER, DRIVER_PIC, 0xF);
	if(!this->irq_p_tun || !this->irq_s_tun) {
		return -1;
	}
	for(int i = 0; i < MAX_ATA_DEVICES; i++) {
		this->devs[i].identify();
		if(this->devs[i].exists()) {
			dprintf("ATA device on bus %d %s found\n", this->devs[i].get_id().bus, this->devs[i].get_id().slave ? "slave" : "master");
		}
	}
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
	this->cur_dev->handle_irq();
//	Send EOI
	this->irq_p_tun->transfer(this, Event(1, 0xE), nullptr);
	return 0;
}

int ATADriver::device_select(ATAId id)
{
	unsigned ind = (id.bus << 1) | id.slave;
	if(ind >= MAX_ATA_DEVICES || !this->devs[ind].exists()) {
		return 1;
	}
	this->cur_dev = this->devs + ind;
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

	outb(this->base_port + 6, 0xE0 | ((this->id.slave & 1) << 4) | ((start_lba >> 24) & 0x0F));
	outb(this->base_port + 2, count);
	outb(this->base_port + 3, start_lba & 0xFF);
	outb(this->base_port + 4, (start_lba >> 8) & 0xFF);
	outb(this->base_port + 5, (start_lba >> 16) & 0xFF);
	outb(this->base_port + 7, 0x20);

	int read = 0;
	while(this->bsy || read < count) {
		if(this->cur_buf_ind > prev_ind) {
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
	inb(this->base_port + 7);
	this->bsy = 1;
	uint16_t *out = this->read_buffer + this->cur_buf_ind * SECTOR_SIZE / 2;
	for(unsigned i = 0; i < SECTOR_SIZE / 2; i++) {
		*(out + i) = inw(this->base_port);
	}
	this->cur_buf_ind++;
	this->bsy = 0;
}

