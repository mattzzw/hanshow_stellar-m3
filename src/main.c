#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

// board specific stuff
#include "uart.h"
#include "led.h"
#include "spi.h"
#include "board.h"


void delay_ms(unsigned int);
void dump_flash(void);

void setup(void)
{
    WDTCTL = WDTPW + WDTHOLD;       // Stop the watchdog timer
    BCSCTL2 = 0;                    // DCOCLK --> MCLK + SCLK, ~1MHz
    
    setup_rgb_led();
    setup_uart();
    setup_spi();
}

 
int main(void)
{
    uint8_t str[128];

    setup();

    //uart_putstring("Hello, World!\n\r");
    //sprintf(str, "Devide ID: 0x%02x\r\n", spi_release_pwrdn_get_dev_id());
    //uart_putstring(str);
    dump_flash();

    while (1)
    {          
        set_rgb_led(1, 1, 1);    
        return 0;   
    }
}

// ---------------------------------------------------

void delay_ms(unsigned int d)
{
    unsigned int i;
    for (i = 0; i < d; i++)
        __delay_cycles(1090);       // Delay ~1ms
}

void dump_flash(void)
{
    u_int8_t data[128];
    uint8_t str[128];
    u_int16_t i;
    u_int32_t a;

    for(a = 0; a < (256L * 1024L); a=a+128){
            spi_read(a, data, 128);

            for(i = 0; i < 128; i++){
                if( i % 8 == 0){
                    uart_putstring("\r\n");
                    sprintf(str, "0x%05lx: ", i+a);
                    uart_putstring(str);
                }
                sprintf(str, "0x%02x ", data[i]);
                uart_putstring(str);
            }
            toggle_led('r');
        }
}