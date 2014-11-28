#include "console.h"
#include "idt.h"
#include "pic.h"
#include "cmd.h"
#include "serial.h"
#include "paging.h"

int
kernel_main(void)
{
    kclear_screen();

    pic_init(0x20, 0x28);
    load_idt();
    init_serial();
    init_pages();
    kprintf("All inited\n");

    while (1)
        cmd();

    return 0;
}
