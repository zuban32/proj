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

#define ISR_dbz 0x0
#define ISR_df 0x8
#define ISR_kbd 0x21
#define ISR_PIT 0x20

void pic_sendEOI(uint8_t);
void pic_remap(uint8_t, uint8_t);

#endif