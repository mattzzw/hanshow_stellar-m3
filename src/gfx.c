#include <stdint.h>
#include "gfx.h"


void gfx_pixel(volatile uint8_t *buf, uint8_t x, uint8_t y, uint8_t c)
{
    // arr_n = (x * 13) + trunc(y / 8) 
    // bit = y % 8

    uint16_t    n_elem;
    uint8_t     bit;
    uint8_t     buff_byte;

    bit = y % 8;

    n_elem = (x * GFX_BUF_HEIGHT / 8) + (int) (y / 8);
    //if(y % 8)
    //    n_elem++;

    buff_byte = buf[n_elem];
    if(c){
        // clear pixel (1=w)
        buff_byte |= (0x80 >> bit);
    } else {
        // set pixel (0=b)
        buff_byte &=  ~(0x80 >> bit);
    }
    
    buf[n_elem] = buff_byte;
}

void gfx_fill_buf(volatile uint8_t * buf, uint8_t c)
{
    uint8_t b;

    if (c){
        b=0xff;     
    } else {
        b=0x00;
    }

    for( int x = 0; x <GFX_BUF_WIDTH * GFX_BUF_HEIGHT / 8; x++){
        buf[x] = b;
    }
}