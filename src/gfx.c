#include <stdint.h>
#include <string.h>
#include "gfx.h"

void gfx_render_str(volatile uint8_t *buf, const char bitmap[][8], const char *str, uint8_t x, uint8_t y, uint8_t c)
{
int n;
int ch;

for(n=0; n < strlen(str); n++){
    ch = str[n];
    gfx_render_char(buf, bitmap[ch], x + (n*9), y, c);
    }
}

void gfx_render_char(volatile uint8_t *buf, const char *bitmap, uint8_t x, uint8_t y, uint8_t c){
    int xoffs,yoffs;
    int set;
    for (yoffs=0; yoffs < 8 ; yoffs++) {
        for (xoffs=0; xoffs < 8; xoffs++) {
            set = bitmap[7-yoffs] & 1 << xoffs;
            if(set)
                c = 1;
                else
                c = 0;
            gfx_pixel(buf, x+xoffs, y+yoffs, c);
        }
    }
}

void gfx_pixel(volatile uint8_t *buf, uint8_t x, uint8_t y, uint8_t c)
{
    uint16_t    n_elem;
    uint8_t     bit;
    uint8_t     buff_byte;

    bit = y % 8;

    n_elem = (x * GFX_BUF_HEIGHT / 8) + (int) (y / 8);
    buff_byte = buf[n_elem];
    if(c){
        // c=1, set a black pixel, clear bit (black=0)
        buff_byte &= ~(0x80 >> bit);
    } else {
        // c=0, set white pixl (white=1)
        buff_byte |=  (0x80 >> bit);
    }
    
    buf[n_elem] = buff_byte;
}

void gfx_fill_buf(volatile uint8_t * buf, uint8_t c)
{
    uint8_t b;

    if (c){
        b=0x00; // c=1, set pixel to black, write 0    
    } else {
        b=0xff; // c=0, clear pixel to white, write 1
    }

    for( int x = 0; x < (GFX_BUF_WIDTH * GFX_BUF_HEIGHT / 8); x++){
        buf[x] = b;
    }
}