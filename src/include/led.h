#define G_LED_PIN BIT2 // Define LED pin as P1.2
#define B_LED_PIN BIT0 // Define LED pin as P2.0
#define R_LED_PIN BIT0 // Define LED pin as P4.0

#define G_LED_PORT P1OUT
#define B_LED_PORT P2OUT
#define R_LED_PORT P4OUT

void set_rgb_led(unsigned char red, unsigned char green, unsigned char blue);
void setup_rgb_led(void);
void toggle_led(unsigned char l);
