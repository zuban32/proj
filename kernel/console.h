#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#define VGA_MEM (char *)0xb8000
#define VGA_MODE 0x7
#define DISPLAY_WIDTH 80
#define DISPLAY_HEIGHT 25

static unsigned cur_pos = 0;

void clear_screen(void);
void backspace(void);
void prints(char *str);
void printint(int c, int base);
void putc(char c);

#endif