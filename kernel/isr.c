#include "isr.h"
#include "pic.h"
#include "console.h"
#include "serial.h"

extern char *cur_buf;
extern const char scancodes[];
extern char input_on;

void
divz_hndl(void)
{
	kprintf("Division by zero\n");
	while(1);
}

void
kbd_hndl(void)
{
	uint8_t sc = inb(0x60);

	// for(int i = 0x30; i < 0x39; i++)
	// 	kprintf("sc[%x] = %c\n", i, scancodes[i]);
	if(sc > 0 && sc < 0x40)
	{
		if(input_on)
		{
			if(sc != 0x1c)
			{
				if(sc != 0xe)
					*cur_buf++ = scancodes[sc];
			}
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
	kprintf("Double fault\n");
	while(1);
}

void
pf_hndl(void)
{
	kprintf("Page fault\n");
	while(1);
}

void
gpf_hndl(void)
{
	kprintf("GP fault\n");
	while(1);
}

void
com_hndl(void)
{
	// asm("cli");
	char c = read_serial();
	if(input_on)
		{
			if(c != '\n' && c != '\r')
			{
				if(c != 0x7f)
					*cur_buf++ = c;
			}
			else
			{
				*cur_buf++ = 0;
				input_on = 0;
			}
		}
	switch(c)
		{
			case '\r':
			case '\n':
				kendline();
				break;
			case 0x7f:
				kbackspace();
				break;
			default:
				kputc(c, 0);

		}
	pic_sendEOI(1);
	// asm("sti");
}