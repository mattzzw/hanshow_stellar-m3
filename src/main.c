#include <msp430.h>
#include <msp430g2755.h>
#include <stdint.h>


#define G_LED_PIN BIT2 // Define LED pin as P1.2
#define B_LED_PIN BIT0 // Define LED pin as P2.0
#define R_LED_PIN BIT0 // Define LED pin as P4.0

#define RED_LED   0
#define BLUE_LED  1
#define GREEN_LED 2

void delay_ms(unsigned int);

void setup(void){

    WDTCTL = WDTPW + WDTHOLD;       // Stop the watchdog timer

    P1DIR |= G_LED_PIN;               // Set P1.2 as output
    P1OUT |= G_LED_PIN;              // Turn off the LED initially

    P2DIR |= B_LED_PIN;
    P2OUT |= B_LED_PIN;

    P4DIR |= R_LED_PIN;
    P4OUT |= R_LED_PIN;

    // Setup UART, TX on P3.4
    // Not using calibration data as it was erased in mass erase.
    // If CALDCO is being used, adjust UCA0BR0 to 4.
    if (CALBC1_1MHZ != 0xff)  BCSCTL1 = CALBC1_1MHZ;
    if (CALDCO_1MHZ != 0xff)  DCOCTL = CALDCO_1MHZ;

    P3SEL |= BIT4;
    P3SEL2 &= BIT4;

#if 1
    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 = 0;
    UCA0CTL1 |= UCSSEL_2;

    UCA0BR0 = 7;            // Set baud rate to 9600 with 1.05MHz clock
    UCA0BR1 = 0;
    UCA0MCTL = (8 << 4) + UCOS16;

    UCA0CTL1 &= ~UCSWRST;    
#endif

}

void print_string(const char *string)
{
    while (*string)
    {
        UCA0TXBUF = *string++;  // Transmit character
        while (!(IFG2 & UCA0TXIFG))
            ;  // Wait for TX buffer to be ready
    }
}

void ctrl_rgb_led(unsigned char red, unsigned char green, unsigned char blue) {
    // Set the output levels based on the intensity values
    if (green > 0) {
        P1OUT &= ~BIT2;  // Turn on green LED
    } else {
        P1OUT |= BIT2; // Turn off green LED
    }

    if (blue > 0) {
        P2OUT &= ~BIT0;  // Turn on blue LED
    } else {
        P2OUT |= BIT0; // Turn off blue LED
    }

    if (red > 0) {
        P4OUT &= ~BIT0;  // Turn on red LED
    } else {
        P4OUT |= BIT0; // Turn off red LED
    }
}
int main(void)
{
    setup();
    print_string("Setup done.\n\r");

    while (1)
    {
        ctrl_rgb_led(1, 0, 0);
        delay_ms(200);               // Delay for a while
        ctrl_rgb_led(0, 1, 0);
        delay_ms(200);               // Delay for a while
        ctrl_rgb_led(0, 0, 1);
        delay_ms(200);               // Delay for a while
    }

    return 0;
}

void delay_ms(unsigned int d)
{
    unsigned int i;
    for (i = 0; i < d; i++)
        __delay_cycles(495);       // Delay ~1ms
}

