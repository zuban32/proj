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

void kendline(void);
void kclear_screen(void);
void kbackspace(void);
void kprints(char *, char );
void kprintint(int , int , char);
void kputc(char , char);
void kprintf(char , const char *, ...);

#endif