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

    // toggle_led('r');
    // delay_ms(200);
    // toggle_led('r');

    setup_uart();
    setup_spi();
    epd_setup_pins();
}

 
int main(void)
{
    uint8_t str[128];

    setup();


    epd_reset();
    uart_putstring("EPD init...\r\n");
    epd_init();
    uart_putstring("Done.\r\nEPD clear disp...\r\n");
    epd_clear_disp();
    uart_putstring("Done.\r\n");
    uart_putstring("Done.\r\nEPD going asleep...\r\n");
    epd_sleep();
    uart_putstring("Done.\r\nZzzzz....\r\n");


    while (1)
    {          
        toggle_led('r');
        delay_ms(500);
    }

    set_rgb_led(1,1,1);
    return 0;   
}

// ---------------------------------------------------

// Timer A0 interrupt service routine

__attribute__ ( ( interrupt( TIMER0_A0_VECTOR ) ) )
void TIMER1_A0_ISR( void )
{
    P1OUT ^= BIT0; // Toggle P1.0 (LED)
    //toggle_led('b');
}

