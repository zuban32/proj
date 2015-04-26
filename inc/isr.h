#ifndef ISR_H_INCLUDED
#define ISR_H_INCLUDED

#include "common.h"

#define MAX_ISR 0x40

// exceptions
#define ISR_DE      0x0
#define ISR_DB      0x1
#define ISR_NMI     0x2
#define ISR_BP      0x3
#define ISR_OF      0x4
#define ISR_BR      0x5
#define ISR_UD      0x6
#define ISR_NM      0x7
#define ISR_DF      0x8
#define ISR_TS      0xA
#define ISR_NP      0xB
#define ISR_SS      0xC
#define ISR_GP      0xD
#define ISR_PF      0xE
#define ISR_MF      0x10
#define ISR_AC      0x11
#define ISR_MC      0x12
#define ISR_XM      0x13
#define ISR_VE      0x14
#define ISR_SX      0x30

// IRQs (mapped already)
#define ISR_PIT     0x20
#define ISR_KBD     0x21
#define ISR_COM1    0x24

typedef struct
{
    int32_t intno;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t orig_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t err_code;
    uint32_t ret_eip;
    uint32_t ret_cs;
    uint32_t eflags;
} Intframe;

void printf_intframe(Intframe *);

void global_handler(Intframe *);

void divz_hndl(void);
void kbd_hndl(void);
void df_hndl(void);
void gpf_hndl(void);
void pf_hndl(Intframe *);
void com_hndl(void);

// extern void pf_start(void);
// extern void gpf_start(void);
// extern void kbd_start(void);
// extern void com_start(void);

// static uintptr_t isr[MAX_ISR] =
// {
//  (uintptr_t)&divz_hndl, 0, 0, 0, 0, 0, 0, 0,                                     //0x0
//  (uintptr_t)&df_hndl, 0, 0, 0, 0, (uintptr_t)&gpf_start, (uintptr_t)&pf_start, 0,
//  0, 0, 0, 0, 0, 0, 0, 0,                                                         //0x10
//  0, 0, 0, 0, 0, 0, 0, 0,
//  0, (uintptr_t)&kbd_start, 0, 0, (uintptr_t)&com_start
// };

#endif

