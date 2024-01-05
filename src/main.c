#include <msp430.h>
#include <stdint.h>

#include "tools.h"
//#include "uart.h"
#include "led.h"
#include "spi.h"
#include "epd.h"
#include "gfx.h"


static volatile uint8_t buf[EPD_WIDTH * EPD_HEIGHT / 8];


void setup(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop the watchdog timer
    BCSCTL2 = 0;                    // DCOCLK --> MCLK + SCLK, ~1MHz
    
    // switch off radio for now
    P1DIR |= BIT3;
    P1OUT |= BIT3;

    setup_rgb_led();

    // toggle_led('r');
    // delay_ms(200);
    // toggle_led('r');

//    setup_uart();
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

//    epd_clear_disp();
//    epd_update_display();



    gfx_fill_buf(buf, 0);

    for( int x = 0; x < 100; x++){
            gfx_pixel(buf, x, x, 1);
    }
    epd_wait_busy();

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


