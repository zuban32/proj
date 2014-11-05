#ifndef PIC_H_INCLUDED
#define PIC_H_INCLUDED

#include "common.h"

#define PIC_M_CMD 0x20
#define PIC_M_DATA 0x21
#define PIC_S_CMD 0xA0
#define PIC_S_DATA 0xA1

#define PIC_EOI 0x20

#define PIC_INIT 0x11
#define ICW4_80386 0x1

void pic_sendEOI(uint8_t);
void pic_init(uint8_t, uint8_t);

#endif