#ifndef KBD_H_INCLUDED
#define KBD_H_INCLUDED

#define BUF_SIZE 4096

#define KEY_NUM 0x60
#define KEY_LEN 0x4

void init_kbd(void);
void clear_buf(void);

char get_scancode(uint8_t sc);
uint8_t input_is_on(void);
void set_input_status(uint8_t input);
char *kbd_get_cur_buf(void);
char *kbd_get_buf(void);
void set_cur_buf(char *buf);

#endif

