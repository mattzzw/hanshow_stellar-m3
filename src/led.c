#include <msp430.h>
#include "led.h"

void setup_rgb_led(void){
    // RGB LED
    P1DIR |= G_LED_PIN;               // Set P1.2 as output
    P1SEL &= ~G_LED_PIN;              // I/O pin
    P1OUT |= G_LED_PIN;               // Turn off the LED initially

    P2DIR |= B_LED_PIN;
    P2SEL &=  ~B_LED_PIN;
    P2OUT |= B_LED_PIN;

    P4DIR |= R_LED_PIN;
    P4SEL &= ~R_LED_PIN;
    P4OUT |= R_LED_PIN;
}

void set_rgb_led(unsigned char red, unsigned char green, unsigned char blue) {
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

void toggle_led(unsigned char l)
{
    switch(l) {
        case 'r': R_LED_PORT ^= R_LED_PIN; break;
        case 'g': G_LED_PORT ^= G_LED_PIN; break;
        case 'b': B_LED_PORT ^= B_LED_PIN; break;
        default: break;
    }
}