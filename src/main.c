#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

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
    setup_spi();
    epd_setup_pins();
    epd_reset();
    epd_init();

}

 
int main(void)
{
    char str[128];

    uint8_t rbuf[10];

    uint8_t wbuf[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};

    uint8_t i;

    setup();

  
    spi_read(0x00, rbuf, 10);

    for(i=0; i<10; i++){
        snprintf(str, 128, "0x%02x ", rbuf[i]);
        uart_putstring(str);
    }
    uart_putstring("\r\n");

    spi_sector_erase(0x00);
    spi_write(0x00, wbuf, 3);


   spi_read(0x00, rbuf, 10);

    for(i=0; i<10; i++){
        snprintf(str, 128, "0x%02x ", rbuf[i]);
        uart_putstring(str);
    }
    uart_putstring("\r\n");


//    spi_write(0x00, write, 1);
//    spi_read(0x00, read, 1);

  //gfx_fill_buf(buf, 0);
    //snprintf(str, 128, "0x%02x", read[0]);
    //gfx_render_str(buf, font8x8_basic, str, 0, GFX_BUF_HEIGHT-20, 1, 2);
//    epd_write_buf(buf);
//    epd_update_display();
 

//    epd_sleep();


    while (1)
    {    
      
       // toggle_led('r');
        //delay_ms(500);
    }

    set_rgb_led(1,1,1);
    return 0;   
}

// ---------------------------------------------------


