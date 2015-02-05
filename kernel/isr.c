#include <inc/isr.h>
#include <inc/pic.h>
#include <inc/console.h>
#include <inc/serial.h>
#include <inc/paging.h>
#include <inc/kbd.h>

extern char *cur_buf;
extern char kbd_buf[];
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
                if (cur_buf - kbd_buf == BUF_SIZE)
                    cur_buf = kbd_buf;
            }
            else
            {
                *cur_buf++ = 0;
                if (cur_buf - kbd_buf == BUF_SIZE)
                    cur_buf = kbd_buf;
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

static int gpf_count = 0;

void
pf_hndl(void)
{
    // asm volatile("pushal\n\t");
    gpf_count = 0;
    kprintf("\nPage fault\n");
    int err_code = 0, ret_eip = 0, err_addr = 0;

    asm volatile("movl 80(%%esp), %%eax\n\t":"=a"(err_code));
    asm volatile("movl 84(%%esp), %%eax\n\t":"=a"(ret_eip));

    kprintf("error: ");

    kprintf((!(err_code & PAGE_U)) ? "kernel "       : "user ");
    kprintf((!(err_code & PAGE_W)) ? "read "         : "write ");
    kprintf((!(err_code & PAGE_P)) ? "non-present "  : "present ");

    kprintf("\n");

    asm volatile("movl %%cr2, %%eax\n\t":"=a"(err_addr));
    kprintf("fault addr = %x\n\n", err_addr);

    pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] |= PAGE_U | PAGE_W | PAGE_P;
    // asm volatile("add $4, %esp\n\t"
    //     "popal\n\t"
    //     "iret\n\t");
}

void
gpf_hndl(void)
{
    gpf_count++;
    kprintf("GP fault\n");
}

void
com_hndl(void)
{
    char c = read_serial();
    if (input_on)
    {
        if (c != '\n' && c != '\r')
        {
            if (c != 0x7f)
                *cur_buf++ = c;
            if (cur_buf - kbd_buf == BUF_SIZE)
                cur_buf = kbd_buf;
        }
        else
        {
            *cur_buf++ = 0;
            input_on = 0;
            if (cur_buf - kbd_buf == BUF_SIZE)
                cur_buf = kbd_buf;
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
    pic_sendEOI(4);
}