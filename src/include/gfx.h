#include "epd.h"

// 0,0 is where the LED is located, 90° rotated
#define GFX_BUF_HEIGHT EPD_WIDTH
#define GFX_BUF_WIDTH  EPD_HEIGHT

void gfx_pixel(volatile uint8_t *buf, uint8_t x, uint8_t y, uint8_t c);
void gfx_fill_buf(volatile uint8_t *, uint8_t);

