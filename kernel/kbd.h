#ifndef KBD_H_INCLUDED
#define KBD_H_INCLUDED

#define KEY_NUM 0xD8
#define KEY_LEN 0x4

const char *scancode[KEY_NUM] = { " ",
	"0","1","2","3","4","5","6","7","8","9"
};

void fill_scancodes(void);

#endif