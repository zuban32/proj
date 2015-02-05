#include <inc/console.h>
#include <inc/common.h>
#include <inc/serial.h>

static unsigned cur_pos = 0;
static unsigned cur_bound = 0;

extern char kbd_buf[];
extern char *cur_buf;

void
kendline(void)
{
    cur_pos = DISPLAY_WIDTH * (cur_pos / DISPLAY_WIDTH + 1);

    write_serial('\n');
}

void
kclear_screen(void)
{
    int sz = 2 * DISPLAY_WIDTH * DISPLAY_HEIGHT;
    char *mem = VGA_MEM;
    while (sz--)
        *mem++ = 0;
    cur_pos = 0, cur_bound = 0;
}

void
kbackspace(void)
{
    if (cur_pos <= cur_bound)
        return;
    if (cur_pos >= 0 && cur_pos < DISPLAY_HEIGHT * DISPLAY_WIDTH)
    {
        char *mem = VGA_MEM + ((cur_pos -= 1) << 1);
        *mem++ = 0, *mem = 0;
    }
    write_serial('\b');
    *(cur_buf -= (cur_buf > kbd_buf) ? 1 : 0) = 0;
}

void
kprints(char *str)
{
    while (*str)
        kputc(*str++, 1);
}

void
kprintint(int c, int base, char is_u)
{
    if (c < 0 && is_u)
        kputc('-', 1), c = -c;
    // int res = c;
    uint32_t res = c, tmp = res, mult = 1;

    do
        mult *= base;
    while (tmp /= base);
    mult /= base;

    do
    {
        int tmp = res / mult + '0';
        kputc(res / mult + ((tmp > '9') ? 'a' - 0xa : '0'), 1);
        res %= mult;
    }
    while (mult /= base);
}

void
kprintf(const char *fstr, ...)
{
    if (!fstr)
        kprintf("Format string empty\n");       //return error code
    va_list p;
    va_start(p, fstr);
    int d;
    uint32_t x;
    char *s, c;

    for (char *str = fstr; *str; str++)
    {
        if (*str == '%')
        {
            switch (*++str)
            {
            case 'b':
                x = va_arg(p, uint32_t);
                kprintint(x, 2, 1);
                break;
            case 'd':
                d = va_arg(p, int);
                kprintint(d, 10, 0);
                break;
            case 'x':
                x = va_arg(p, uint32_t);
                kprintf("0x");
                kprintint(x, 16, 1);
                break;
            case 's':
                s = va_arg(p, char *);
                kprints(s);
                break;
            case 'c':
                c  = va_arg(p, int);
                kputc(c, 1);
                break;
            default:
                break;
            }
        }
        else if (*str == '\n')
            kendline();
        else
            kputc(*str, 1);
    }
    va_end(p);
}

void
kputc(char c, char move_bound)
{
    if (cur_pos < DISPLAY_WIDTH * DISPLAY_HEIGHT && cur_pos >= 0)
    {
        char *mem = VGA_MEM + (cur_pos << 1);
        *mem++ = c;
        *mem++ = VGA_MODE;
        cur_pos ++;
        if (move_bound)
            cur_bound = cur_pos;
    }
    write_serial(c);
}