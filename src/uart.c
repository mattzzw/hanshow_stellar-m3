/******************************************************************************
 * Bit banged super simple version of an I-just-need-debug-output-UART-TX
 * 
 ******************************************************************************/
   
#include  "msp430.h"
#include <stdint.h>
#include "uart.h"


static volatile unsigned int tx_byte;   // Value sent over UART when Transmit() is called

void setup_uart(void)
{
  P1DIR  |= TXD;                              // output
  P1SEL  &= ~TXD;                             // P1.1 I/O
  P1OUT |= TXD;
}
 
// Function Transmits Character from tx_byte 
void uart_putc(uint8_t c)
{ 
  unsigned int tx_bits;

  tx_byte = c;
  tx_byte |= 0x100;                 // Add stop bit to tx_byte (which is logical 1)
  tx_byte = tx_byte << 1;            // Add start bit (which is logical 0)
  for(tx_bits = 10; tx_bits > 0; tx_bits--){
    if (tx_byte & 0x01){
      P1OUT |= TXD;
    } else {
      P1OUT &= ~TXD;
    }
    tx_byte >>= 1;
    __delay_cycles(BAUD_19200);   
  }
  P1OUT |= TXD;                   // Set TXD Idle as Mark = '1'     

}
 
void uart_putstring(const char *string)
{
    while (*string){
        uart_putc(*string++);  // Transmit character
    }
}



