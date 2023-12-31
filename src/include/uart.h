#include  "msp430.h"
#include <stdint.h>

#define TXD         BIT1    // TXD on P1.1
#define BAUD_19200  20      // roughly checked with scope, good enough for 1.05MHz MCLK
#define BAUD_9600   80
  
// Function Definitions
void uart_putstring(const char *string);
void uart_putc(uint8_t);
void setup_uart(void);
