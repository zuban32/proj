#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include <stdint.h>

typedef __builtin_va_list va_list;

#define va_start(ap, last) __builtin_va_start(ap, last)

#define va_arg(ap, type) __builtin_va_arg(ap, type)

#define va_end(ap) __builtin_va_end(ap)

int get_curx(void);
int get_cury(void);
void set_curx(int val);
void set_cury(int val);

void kendline(void);
void kclear_screen(void);
void kbackspace(void);
void kputc(char c, char move_bound);
void kprintf(const char *fstr, ...);

#endif

