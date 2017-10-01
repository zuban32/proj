#include <hw/vesa.h>

static VESADriver ata_driver;

int VESADriver::init()
{
    this->framebuf = reinterpret_cast<uint8_t *>(FRAMEBUFFER_ADDR);
    return 0;
}

int VESADriver::connect_from(Tunnel *t, int data)
{
    if(!t) {
        return -1;
    }
    this->in_tun = t;
    return 0;
}

int VESADriver::handle(Event e, void *ret)
{
    // VESA isn't intended to handle any IRQs
    return 0;
}


