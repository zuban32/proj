#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED

#include "common.h"

#define MAX_ISR 0x40 

#define ISR_ZERO 0x0
#define ISR_DFAULT 0x8
#define ISR_GPFAULT 0xd
#define ISR_PFAULT 0xe
#define ISR_KBD 0x21
#define ISR_PIT 0x20
#define ISR_COM1 0x24

void divz_hndl(void);
void kbd_hndl(void);
void df_hndl(void);
void gpf_hndl(void);
void pf_hndl(void);
void com_hndl(void);

static uintptr_t isr[MAX_ISR] = 
{
	(uintptr_t)&divz_hndl, 0, 0, 0, 0, 0, 0, 0, 					//0x0
	(uintptr_t)&df_hndl, 0, 0, 0, 0, 0, (uintptr_t)&pf_hndl, 0,
	0, 0, 0, 0, 0, 0, 0, 0,										//0x10
	0, 0, 0, 0, 0, 0, 0, 0,
	0, (uintptr_t)&kbd_hndl, 0, 0, (uintptr_t)&com_hndl
};

#endif