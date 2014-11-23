#include "console.h"
#include "common.h"
#include "serial.h"

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
    // kprintf("move bound %s\n", move_bound ? "enabled" : "disabled");
    // kprintf("cur_pos = %d, cur_bound = %d\n", cur_pos, cur_bound);
    if (!cur_pos || cur_pos <= cur_bound)
        return;
    char *mem = VGA_MEM + ((cur_pos -= 1) << 1);
    *mem++ = 0, *mem = 0;
    write_serial('\b');
    *(cur_buf -= (cur_buf > kbd_buf) ? 1 : 0) = 0;
}

void
kprints(char *str, char move_bound)
{
    // char *mem = VGA_MEM + (cur_pos << 1);
    while (*str)
    {
        // *mem++ = *str, *mem++ = VGA_MODE;
        kputc(*str++, 1);
    }
    // cur_pos = (mem - VGA_MEM) >> 1;
    // if(move_bound)
    //  cur_bound = cur_pos;
}

void
kprintint(int c, int base, char move_bound)
{
    if (c < 0)
        kputc('-', move_bound), c = -c;
    char *mem = VGA_MEM + (cur_pos << 1);
    int res = c > 0 ? c : -c;
    int mult = 1, tmp = res;

    do
        mult *= base;
    while (tmp /= base);
    mult /= base;

    do
    {
        int tmp = res / mult + '0';
        kputc(res / mult + ((tmp > '9') ? 'a' - 0xa : '0'), move_bound);
        res %= mult;
    }
    while (mult /= base);
}

void
kprintf(char move_bound, const char *fstr, ...)
{
    if (!fstr)
        kprintf(1, "String empty\n");
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
                kprintint(x, 2, move_bound);
                break;
            case 'd':
                d = va_arg(p, int);
                kprintint(d, 10, move_bound);
                break;
            case 'x':
                x = va_arg(p, uint32_t);
                kprintf(move_bound, "0x");
                kprintint(x, 16, move_bound);
                break;
            case 's':
                s = va_arg(p, char *);
                kprints(s, move_bound);
                break;
            case 'c':
                c  = va_arg(p, int);
                kputc(c, move_bound);
                break;
            default:
                break;
            }
        }
        else if (*str == '\n')
            kendline();
        else
            kputc(*str, move_bound);
    }
    va_end(p);
}

void
kputc(char c, char move_bound)
{
    char *mem = VGA_MEM + (cur_pos << 1);
    *mem++ = c;
    *mem++ = VGA_MODE;
    cur_pos ++;
    if (move_bound)
        cur_bound = cur_pos;
    write_serial(c);
}