#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

#include "tools.h"
#include "uart.h"
#include "led.h"
#include "spi.h"
#include "epd.h"

void setup(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop the watchdog timer
    BCSCTL2 = 0;                    // DCOCLK --> MCLK + SCLK, ~1MHz
    
    setup_rgb_led();
    setup_uart();
    setup_spi();
    epd_setup_pins();
}

 
int main(void)
{
    uint8_t str[128];

    setup();

    toggle_led('r');
    delay_ms(500);
    toggle_led('r');
    uart_putstring("Hello, World!\n\r");
    sprintf(str, "Flash devide ID: 0x%02x\r\n", spi_release_pwrdn_get_dev_id());
    //uart_putstring(str);
    //dump_flash();

    epd_reset();
    epd_init();
    epd_clear_disp();

    while (1)
    {          
        //set_rgb_led(1, 1, 1);    
    }
    return 0;   
}

// ---------------------------------------------------

