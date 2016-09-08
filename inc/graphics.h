#ifndef INC_GRAPHICS_H_
#define INC_GRAPHICS_H_

#include <stdint.h>

// defines for text mode 80x25
#define DISPLAY_TEXT_WIDTH 		80
#define DISPLAY_TEXT_HEIGHT 	25

#define TEXT_VIDEO_MEM 			0xb8000
#define TEXT_COLOR 				0x7

// defines for contigious 1024*768*24 mode
#define VIDEO_MEM 				0xfd000000
#define DISPLAY_PIXEL_WIDTH 	1024
#define DISPLAY_PIXEL_HEIGHT 	768
#define COLOR_DEPTH 			3

enum
{
	BIOS_VESA_GET_CONTROLLER_MODE = 0x4F00,
	BIOS_VESA_GET_MODE_INFO = 0x4F01,
	BIOS_VESA_SET_MODE = 0x4F02
};

enum
{
	VIDEO_TYPE_NONE = 0x00,
	VIDEO_TYPE_COLOUR = 0x20,
	VIDEO_TYPE_MONOCHROME = 0x30,
};

typedef struct Pixel
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} Pixel;

struct VBEInfoBlock
{
	char VbeSignature[4];             // == "VESA"
	uint16_t VbeVersion;                 // == 0x0300 for VBE 3.0
	uint16_t OemStringPtr[2];            // isa vbeFarPtr
	uint8_t Capabilities[4];
	uint16_t VideoModePtr[2];         // isa vbeFarPtr
	uint16_t TotalMemory;             // as # of 64KB blocks
}__attribute__((packed));

typedef struct VBEInfoBlock VBEInfoBlock;

typedef struct ModeInfoBlock
{
	uint16_t attributes;
	uint8_t winA, winB;
	uint16_t granularity;
	uint16_t winsize;
	uint16_t segmentA, segmentB;
	uint32_t realFctPtr;
	uint16_t pitch; // bytes per scanline

	uint16_t Xres, Yres;
	uint8_t Wchar, Ychar, planes, bpp, banks;
	uint8_t memory_model, bank_size, image_pages;
	uint8_t reserved0;

	uint8_t red_mask, red_position;
	uint8_t green_mask, green_position;
	uint8_t blue_mask, blue_position;
	uint8_t rsv_mask, rsv_position;
	uint8_t directcolor_attributes;

	uint32_t physbase;  // LFB address
	uint32_t reserved1;
	uint16_t reserved2;
} ModeInfoBlock;

void draw_letter(int16_t pos_x, int16_t pos_y, uint8_t *letter);
void put_pixel(int x, int y, int r, int g, int b);

int init_vesa(void);

#endif /* INC_GRAPHICS_H_ */
