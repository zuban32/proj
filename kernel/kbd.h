#ifndef KBD_H_INCLUDED
#define KBD_H_INCLUDED

enum scancode
{
	KEY_A = 0x1e,
	KEY_B = 0x30,
	KEY_C = 0x2e,
	KEY_D = 0x20,
	KEY_E = 0x12,
	KEY_F = 0x21,
	KEY_G = 0x22,
	KEY_H = 0x23,
	KEY_I = 0x17,
	KEY_J = 0x24,
	KEY_K = 0x25,
	KEY_L = 0x26,
	KEY_M = 0x32,
	KEY_N = 0x31,
	KEY_O = 0x18,
	KEY_P = 0x19,
	KEY_Q = 0x10,
	KEY_R = 0x13,
	KEY_S = 0x1f,
	KEY_T = 0x14,
	KEY_U = 0x16,
	KEY_V = 0x2f,
	KEY_W = 0x11,
	KEY_X = 0x2d,
	KEY_Y = 0x15,
	KEY_Z = 0x2c,
	KEY_0 = 0xb,
	KEY_1 = 0x2,
	KEY_2 = 0x3,
	KEY_3 = 0x4,
	KEY_4 = 0x5,
	KEY_5 = 0x6,
	KEY_6 = 0x7,
	KEY_7 = 0x8,
	KEY_8 = 0x9,
	KEY_9 = 0xa
};

void kbd_ack(void);
void kbd_init(void);
char get_key(void);

#endif