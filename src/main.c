#include <msp430.h>
#include <stdint.h>
#include <stdio.h>

// board specific stuff
#include "uart.h"
#include "led.h"
#include "spi.h"
#include "board.h"
#include "epd.h"


void delay_ms(unsigned int);
void dump_flash(void);

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

    while (1)
    {          
        //set_rgb_led(1, 1, 1);    
    }
    return 0;   
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
    u_int16_t chunk_size = 256;
    u_int8_t data[chunk_size];
    uint8_t str[128];
    u_int16_t i;
    u_int32_t a;

    for(a = 0; a < (256L * 1024L); a=a+chunk_size){
            spi_read(a, data, chunk_size);

            for(i = 0; i < chunk_size; i++){
                if( i % 16 == 0){
                    uart_putstring("\r\n");
                }
                sprintf(str, "%02x ", data[i]);
                uart_putstring(str);
            }
            toggle_led('r');
        }
    uart_putstring("\r\n");

}