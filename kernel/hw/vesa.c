#include <inc/idt.h>
#include <inc/console.h>
#include <inc/graphics.h>
#include <inc/font.h>

#if 0
static uint16_t detect_bios_area_hardware(void)
{
	const uint16_t* bda_detected_hardware_ptr = (const uint16_t*) 0x410;
	return *bda_detected_hardware_ptr;
}
static enum video_type get_bios_area_video_type(void)
{
	return (enum video_type) (detect_bios_area_hardware() & 0x30);
}
#endif

void draw_letter(int16_t pos_x, int16_t pos_y, uint8_t *letter)
{
	for (int i = 0; i < FONT_HEIGHT; i++) {
		for (int j = 0; j < FONT_WIDTH; j++) {
			int16_t x = get_curx() * FONT_HEIGHT + (FONT_HEIGHT - i - 1);
			int16_t y = get_cury() * FONT_WIDTH + (FONT_WIDTH - j - 1);
			if (!letter) {
				put_pixel(x, y, 0, 0, 0);
			} else if (letter[j] & (1 << i)) {
				put_pixel(x, y, 0xff, 0xff, 0xff);
			}
		}
	}
}

void put_pixel(int x, int y, int r, int g, int b)
{
	uintptr_t place = (x + y * DISPLAY_PIXEL_WIDTH) * COLOR_DEPTH;

	uint8_t *mem = (uint8_t *) VIDEO_MEM + place;

	mem[0] = r;
	mem[1] = g;
	mem[2] = b;
}

int init_vesa(void)
{
#if 0
	kprintf("Video type = %x\n", get_bios_area_video_type());
#endif
	return 0;
}

