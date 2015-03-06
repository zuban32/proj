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

const char *exception_names[] =
{
    "Divide error",
    "Debug Exception",
    "NMI Interrupt",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalide Opcode",
    "Coprocessor Not Available",
    "Double Fault",
    "Coprocessor segment overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection",
    "Page Fault",
    "x87 FPU Floating-Point Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception"
};

const char *eflags_flagnames[] =
{
    "CF",   NULL,   "PF",       NULL,       "AF",
    NULL,   "ZF",   "SF",       "TF",       "IF",
    "DF",   "OF",   "IOPL1",    "IOPL2",    "NT",
    NULL,   "RF",   "VM",       "AC",       "VIF",
    "VIP",  "ID"
};

void
print_intframe(Intframe *iframe)
{
    if (iframe->intno == ISR_KBD || iframe->intno == ISR_COM1)
        return;
    kprintf("\n-------------------------\n");
    kprintf("INTFRAME\n%d - ", iframe->intno);
    if (iframe->intno >= 0 && iframe->intno < 21 && iframe->intno != 9 && iframe->intno != 15)
        kprintf("%s\n", exception_names[iframe->intno]);
    else
        kprintf("User-Defined Interrupt\n");
    kprintf("ret_eip = %x\nret_cs = %x\neflags = %00b\n", iframe->ret_eip,
            iframe->ret_cs,
            iframe->eflags);

    kprintf("[ ");   //just like gdb shows enable flags
    for (int i = 0; i < 22; i++)
    {
        if (i == 12)
        {
            kprintf("IOPL=%d ", 2 * (iframe->eflags & (1 << 13)) +
                    (iframe->eflags & (1 << 12)) );
            i++;
        }
        if (eflags_flagnames[i] && (iframe->eflags & (1 << i)))
            kprintf("%s ", eflags_flagnames[i]);
    }
    kprintf("]\n");

    kprintf("err_code = %03b  ", iframe->err_code & 0x7);
    if (iframe->intno == ISR_PF)
    {
        kprintf((!(iframe->err_code & PAGE_U)) ? "kernel "       : "user ");
        kprintf((!(iframe->err_code & PAGE_W)) ? "read "         : "write ");
        kprintf((!(iframe->err_code & PAGE_P)) ? "non-present\n"  : "present\n");

        uint32_t err_addr = 0;
        asm volatile("movl %%cr2, %%eax\n\t":"=a"(err_addr));
        kprintf("fault addr = %x", err_addr);
    }
    kprintf("\n-------------------------\n\n");
}

void
global_handler(Intframe *iframe)
{
    print_intframe(iframe);

    switch (iframe->intno)
    {
    case ISR_DE:
        divz_hndl();
        break;
    case ISR_PF:
        pf_hndl(iframe);
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
        kprintf("ISR for int num %d doesn't exist\n", iframe->intno);
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
pf_hndl(Intframe *iframe)
{
    pgtbl[PDX(CHECKADDR)][PTX(CHECKADDR)] |= PAGE_U | PAGE_W | PAGE_P;
}

void
gpf_hndl(void)
{
    // kprintf("GP fault\n");
    // while(1);
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