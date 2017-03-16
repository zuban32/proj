#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED

#include <inc/common.h>
enum
{
	// exceptions
	ISR_DE = 0x0,
	ISR_DB = 0x1,
	ISR_NMI = 0x2,
	ISR_BP = 0x3,
	ISR_OF = 0x4,
	ISR_BR = 0x5,
	ISR_UD = 0x6,
	ISR_NM = 0x7,
	ISR_DF = 0x8,
	ISR_TS = 0xA,
	ISR_NP = 0xB,
	ISR_SS = 0xC,
	ISR_GP = 0xD,
	ISR_PF = 0xE,
	ISR_MF = 0x10,
	ISR_AC = 0x11,
	ISR_MC = 0x12,
	ISR_XM = 0x13,
	ISR_VE = 0x14,
//	ISR_SX = 0x30,

	// already mapped
	ISR_PIT = 0x20,
	ISR_KBD = 0x21,
	ISR_COM1 = 0x24,
	ISR_ATA = 0x2E,
	ISR_SYSCALL = 0x80,
	MAX_ISR
};

typedef struct
{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t orig_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint16_t es;
	uint16_t padd1;
	uint16_t ds;
	uint16_t padd2;
	uint32_t intno;
	uint32_t err_code;
//	below provided by x86
	uintptr_t ret_eip;
	uint32_t ret_cs;
	uint32_t eflags;
	uintptr_t esp;
	uint32_t ss;
} Intframe;

void printf_intframe(Intframe *iframe);

void divz_hndl(void);
void kbd_hndl(void);
void df_hndl(void);
void gpf_hndl(void);
void pf_hndl(Intframe *iframe);
void com_hndl(void);

#endif

