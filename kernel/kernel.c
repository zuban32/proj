#include <inc/console.h>
#include <inc/idt.h>
#include <inc/pic.h>
#include <inc/cmd.h>
#include <inc/serial.h>
#include <inc/paging.h>

int
kernel_main(void)
{
	kprintf("%x\n", 0x0);
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
