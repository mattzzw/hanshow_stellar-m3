#include <msp430.h>
#include <stdint.h>
#include "tools.h"
#include "uart.h"
#include "led.h"
#include "spi.h"
#include "epd.h"
#include "gfx.h"
#include "font8x8_basic.h"


static volatile uint8_t buf[EPD_WIDTH * EPD_HEIGHT / 8];

void setup(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop the watchdog timer
    BCSCTL2 = 0;                    // DCOCLK --> MCLK + SCLK, ~1MHz
    
    // switch off radio for now
    P1DIR |= BIT3;
    P1OUT |= BIT3;

    setup_rgb_led();

    setup_uart();
    //setup_spi();
    epd_setup_pins();
    epd_reset();
    epd_init();

}

 
int main(void)
{
    //uint8_t str[128];

    setup();
    toggle_led('b');

    gfx_fill_buf(buf, 0);

    gfx_render_str(buf, font8x8_basic, "Hello -", 0, GFX_BUF_HEIGHT-10, 1, 1);
    gfx_render_str(buf, font8x8_basic, "I am", 20, GFX_BUF_HEIGHT/2, 1, 2);
    gfx_render_str(buf, font8x8_basic, "alive!", 40, 0, 1, 3);

    epd_write_buf(buf);
    epd_update_display();
    toggle_led('b');

    epd_sleep();


    while (1)
    {    
      
        toggle_led('r');
        delay_ms(500);
    }

    set_rgb_led(1,1,1);
    return 0;   
}

// ---------------------------------------------------


