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
global_handler(Intframe *tf)
{
    switch (tf->trapno)
    {
    case ISR_DE:
        divz_hndl();
        break;
    case ISR_PF:
        pf_hndl(tf);
        break;
    case ISR_KBD:
        kbd_hndl();
        break;
    case ISR_DF:
        df_hndl();
        break;
    case ISR_GP:
        gpf_hndl();
        break;
    case ISR_COM1:
        com_hndl();
        break;
    default:
        kprintf("ISR for int num %d doesn't exist\n", tf->trapno);
        break;
    }
}

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
                if (cur_buf - kbd_buf == BUF_SIZE)
                    cur_buf = kbd_buf;
                if (sc != 0xe)
                    *cur_buf++ = scancodes[sc];
            }
            else
            {
                if (cur_buf - kbd_buf == BUF_SIZE)
                    cur_buf = kbd_buf;
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

void
pf_hndl(Intframe *tf)
{
    // asm volatile("pushal\n\t");
    kprintf("\nPage fault\n");

    // kprintf("%x %x\n", tf->ret_eip, tf->err_code);
    uint32_t err_addr = 0;

    kprintf((!(tf->err_code & PAGE_U)) ? "kernel "       : "user ");
    kprintf((!(tf->err_code & PAGE_W)) ? "read "         : "write ");
    kprintf((!(tf->err_code & PAGE_P)) ? "non-present\n"  : "present\n");

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
    kprintf("GP fault\n");
}

void
com_hndl(void)
{
    char c = read_serial();
    if (input_on)
    {
        switch (c)
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
}