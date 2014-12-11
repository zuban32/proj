#include <inc/isr.h>
#include <inc/pic.h>
#include <inc/console.h>
#include <inc/serial.h>
#include <inc/paging.h>

extern char *cur_buf;
extern const char scancodes[];
extern char input_on;

void
divz_hndl(void)
{
    kprintf("Division by zero\n");
}

void
kbd_hndl(void)
{
    uint8_t sc = inb(0x60);

    if (sc > 0 && sc < 0x40)
    {
        if (input_on)
        {
            if (sc != 0x1c)
            {
                if (sc != 0xe)
                    *cur_buf++ = scancodes[sc];
            }
            else
            {
                *cur_buf++ = 0;
                input_on = 0;
            }
        }

        switch (sc)
        {
        case 0x1c:
            kendline();
            break;
        case 0xe:
            kbackspace();
            break;
        default:
            kputc(scancodes[sc], 0);
        }
    }
    pic_sendEOI(1);
}

void
df_hndl(void)
{
    kprintf("Double fault\n");
    while (1);
}

extern uint32_t pgtbl[][PGS_NUM];
#define CHECKADDR 0x27ff010

static int gpf_count = 0;

void
pf_hndl(void)
{
    gpf_count = 0;
    kprintf("Page fault\n");
    int res1, res2;
    asm volatile("movl 80(%%esp), %%eax\n\t":"=a"(res1));
    asm volatile("movl 84(%%esp), %%eax\n\t":"=a"(res2));
    kprintf("error (or ret_eip) = %x, ret_eip(or ret_cs) = %x\n", res1, res2);
    pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] |= 1;
    kprintf("Returning\n");
}

void
gpf_hndl(void)
{
	gpf_count++;
    kprintf("GP fault\n");
    int res1, res2;
    asm volatile("movl 80(%%esp), %%eax\n\t":"=a"(res1));
    asm volatile("movl 84(%%esp), %%eax\n\t":"=a"(res2));
    kprintf("error (or ret_eip) = %x, ret_eip(or ret_cs) = %x\n", res1, res2);
    kprintf("Returning\n");
    if (gpf_count >= 3)
        while (1);
}

void
com_hndl(void)
{
    // asm("cli");
    char c = read_serial();
    if (input_on)
    {
        if (c != '\n' && c != '\r')
        {
            if (c != 0x7f)
                *cur_buf++ = c;
        }
        else
        {
            *cur_buf++ = 0;
            input_on = 0;
        }
    }
    switch (c)
    {
    case '\r':
    case '\n':
        kendline();
        break;
    case 0x7f:
        kbackspace();
        break;
    default:
        kputc(c, 0);

    }
    pic_sendEOI(1);
    // asm("sti");
}