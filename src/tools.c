#include <msp430.h>
#include <stdint.h>
//#include "led.h"
//#include "spi.h"
//#include "uart.h"
#include "tools.h"


void delay_ms(unsigned int d)
{
    unsigned int i;
    for (i = 0; i < d; i++)
        __delay_cycles(CYCLES_PER_MS);       // Delay ~1ms
}

/*  not used any more */
/*
void dump_flash(void)
{
    u_int16_t chunk_size = 256;
    u_int8_t data[chunk_size];
    char str[128];
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
*/
