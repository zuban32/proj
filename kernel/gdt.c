#include <inc/gdt.h>
#include <inc/string.h>
#include <inc/console.h>

static tss_entry tss;

static void write_tss(gdt_entry *g)
{
	   // Firstly, let's compute the base and limit of our entry into the GDT.
	   uint32_t base = (uint32_t)&tss;
	   uint16_t limit = sizeof(tss);

	   // Now, add our TSS descriptor's address to the GDT.
	   g->limit_low=limit&0xFFFF;
	   g->base_low=base&0xFFFFFF; //isolate bottom 24 bits
	   g->accessed=1; //This indicates it's a TSS and not a LDT. This is a changed meaning
	   g->read_write=0; //This indicates if the TSS is busy or not. 0 for not busy
	   g->conforming_expand_down=0; //always 0 for TSS
	   g->code=1; //For TSS this is 1 for 32bit usage, or 0 for 16bit.
	   g->always_1=0; //indicate it is a TSS
	   g->DPL=3; //same meaning
	   g->present=1; //same meaning
	   g->limit_high=(limit&0xF0000)>>16; //isolate top nibble
	   g->available=0;
	   g->always_0=0; //same thing
	   g->big=0; //should leave zero according to manuals. No effect
	   g->gran=0; //so that our computed GDT limit is in bytes, not pages
	   g->base_high=(base&0xFF000000)>>24; //isolate top byte.

	   // Ensure the TSS is initially zero'd.
	   kmemset(&tss, 0, sizeof(tss_entry));

	   tss.ss0  = 0x10;  // Set the kernel stack segment.
	   tss.esp0 = 0x900000; // Set the kernel stack pointer.
	   //note that CS is loaded from the IDT entry and should be the regular kernel code segment
}

void init_user_gdt(gdt_entry *gdt)
{
	gdt_entry *code = &gdt[3];
	gdt_entry *data = &gdt[4];

	kmemset(code, 0, sizeof(*code));
	kmemset(data, 0, sizeof(*data));

	code->limit_low = 0xFFFF;
	code->read_write = 1; //make it readable for code segments
	code->code = 1; //this is to signal its a code segment
	code->always_1 = 1;
	code->DPL = 3; //set it to ring 3
	code->present = 1;
	code->limit_high = 0xF;
	code->available = 1;
	code->big = 1; //signal it's 32 bits
	code->gran = 1; //use 4k page addressing
	*data = *code; //copy it all over, cause most of it is the same
//	kmemcpy((char *)data, (char *)code, sizeof(*data));
	data->code = 0; //signal it's not code; so it's data.

	write_tss(&gdt[5]);

	__asm __volatile(
			   "mov $0x2B, %ax\n\t"
			   "ltr %ax\n\t"
			   );
}
