#ifndef INC_HW_VESA_H_
#define INC_HW_VESA_H_

#include <abstract.h>
#include <util/port.h>

// must be tested first!
enum {
    FRAMEBUFFER_ADDR = 0xB8000,
};

class VESADriver: public Unit
{
    Tunnel *in_tun = nullptr;

    uint8_t *framebuf = nullptr;

public:
    VESADriver(): Unit(UNIT_DRIVER, DRIVER_VESA) {}

    int init();
    int connect_from(Tunnel *t, int data);
    int handle(Event e, void *ret);

    ~VESADriver() {}
};

#endif /* INC_HW_VESA_H_ */
