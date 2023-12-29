#include <msp430.h>
#include <stdint.h>

// board specific stuff
#include "uart.h"
#include "led.h"
#include "board.h"


void delay_ms(unsigned int);

void setup(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop the watchdog timer

    // close setup
    BCSCTL2 = 0;  // DCOCLK --> MCLK + SCLK, ~1MHz
    
    setup_rgb_led();
    setup_uart();
}

 
int main(void)
{
    setup();

    uart_putstring("Hello, World!\n\r");

    while (1)
    {        
        toggle_led('r');
        delay_ms(500);
    }

    return 0;
}

void delay_ms(unsigned int d)
{
    unsigned int i;
    for (i = 0; i < d; i++)
        __delay_cycles(495);       // Delay ~1ms
}

