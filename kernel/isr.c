#include <inc/isr.h>
#include <inc/pic.h>
#include <inc/console.h>
#include <inc/serial.h>

extern char *cur_buf;
extern const char scancodes[];
extern char input_on;

void
divz_hndl(void)
{
	kprintf("Division by zero\n");
}

void
kbd_hndl(void)
{
	uint8_t sc = inb(0x60);

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
	// asm volatile ("cli\n\t");
	kprintf("Double fault\n");
	while(1);
}

void
pf_hndl(void)
{
	asm volatile ("cli\n\t");
	kprintf("Page fault\n");
	while(1);
}

void
gpf_hndl(void)
{
	asm volatile ("cli\n\t");
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