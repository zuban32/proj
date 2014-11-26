#include "kbd.h"
#include "common.h"
#include "console.h"
#include "idt.h"
#include "pic.h"
#include "string.h"
#include "cmd.h"
#include "serial.h"

// extern struct PageDir *pgdir;

// extern uint32_t pgdir[PGS_NUM];
// extern uint32_t pgtbl[PGS_NUM][PGS_NUM];


int
kernel_main(void)
{
    // __asm__ volatile("sti\t\n");
    kclear_screen();
    kprintf("%d %x\n", -500, 0xfffaa);

    pic_init(0x20, 0x28);
    kprintf("PIC inited\n");
    load_idt();
    init_serial();

    // int x = 5 / 0;

    kstoi("112515");
    kprintf("All inited exc pages\n");
    init_pages();
    kprintf("All inited\n");

    while (1)
        cmd();

    return 0;
}
