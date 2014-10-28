#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)

#define va_arg(ap, type) __builtin_va_arg(ap, type)

#define va_end(ap) __builtin_va_end(ap)

#define VGA_MEM (char *)0xb8000
#define VGA_MODE 0x7
#define DISPLAY_WIDTH 80
#define DISPLAY_HEIGHT 25

static unsigned cur_pos = 0;

void endline(void);
void clear_screen(void);
void backspace(void);
void prints(char *str);
void printint(int c, int base);
void putc(char c);
void printf(const char *, ...);

#endif