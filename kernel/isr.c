#include "isr.h"
#include "pic.h"
#include "console.h"

extern char *cur_buf;
extern const char scancodes[];
extern char input_on;

void 
divz_hndl(void)
{
	kprintf(1, "Division by zero\n");
	while(1);
}

void
kbd_hndl(void)
{
	uint8_t sc = inb(0x60);	load_idt();

	// for(int i = 0x30; i < 0x39; i++)
	// 	kprintf(1, "sc[%x] = %c\n", i, scancodes[i]);
	if(sc > 0 && sc < 0x40)
	{
		if(input_on)
		{
			if(sc != 0x1c)
				*cur_buf++ = scancodes[sc];
			else
			{
				*cur_buf++ = 0;
				input_on = 0;
			}
		}

		switch(sc)
		{
			case 0x1c:
				kendline();
				break;
			case 0xe:
				kbackspace();
				break;
			default:
				kputc(scancodes[sc], 0);

		}
	}
	pic_sendEOI(1);
}

void
df_hndl(void)
{
	kprintf(1, "Double fault\n");
	while(1);
}

void
pf_hndl(void)
{
	kprintf(1, "Page fault\n");
	while(1);
}