#include  "msp430g2755.h"
#include <stdint.h>


#define     TXD       BIT1   // TXD on P1.1
#define     Bitime    104    //9600 Baud, SMCLK=1MHz (1MHz/9600)=104
  
// Function Definitions
void uart_putstring(const char *string);
void uart_putch(uint8_t);
void setup_uart(void);
