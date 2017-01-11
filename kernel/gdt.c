#include <inc/gdt.h>
#include <inc/string.h>
#include <inc/console.h>

static void write_tss(void)
{
	tss_entry *tss = (tss_entry *)0x7E00;
	kmemset(tss, 0, sizeof(*tss));
	tss->ss0  = 0x10;
	tss->esp0 = 0x900000;
}

void init_user_gdt(void)
{
	write_tss();
	tss_entry *tss = (tss_entry *)0x7E00;
	kprintf("TSS: %x %x\n", tss->ss0, tss->esp0);

	__asm __volatile(
			   "mov $0x28, %ax\n\t"
			   "ltr %ax\n\t"
			   );
}
