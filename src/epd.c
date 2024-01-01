#include <stdint.h>
#include "msp430.h"
#include "epd.h"
#include "led.h"
#include "tools.h"
#include "uart.h"

void epd_setup_pins(void)
{
    //PxSEL are 0 after reset
    
    // // we are going to use P1.0 as gate driver for the charge pump
    // // Configure Timer_A
    // TA0CCR0 = 0;                     // Stop timer
    // TA0CCTL0 |= CCIE;                // Enable interrupt for CCR0
    // TA0CTL = TASSEL_2 | MC_1 | ID_2; // Use SMCLK, Up mode, no division

    // __enable_interrupt();
    // _BIS_SR( GIE); //ENABLE GLOBAL INTERRRUPTS

    // TA0CCR0 = 1000; // start timer

    // configure pin directions
    P1DIR |= EPD_PWR_PIN;
    P2DIR |= EPD_DIN_PIN;
    P2DIR |= EPD_CLK_PIN;
    P3DIR |= EPD_CSN_PIN;
    P3DIR |= EPD_DCn_PIN;
    P4DIR |= EPD_RST_PIN;

    P4DIR &= ~EPD_BUS_PIN; // input
    P4REN |= EPD_BUS_PIN;  // enable pullup/down
    P4OUT |= EPD_BUS_PIN;  // pull-up
    
    P4DIR |= EPD_BS1_PIN;

 

    // switching Q6
    //P1DIR |= BIT3;
    //P1OUT |= BIT3;

    // debug pins
    // P1DIR |= BIT4;
    // P1DIR |= BIT5;
    // P1SEL &= ~BIT4;
    // P1SEL &= ~BIT5;
 
    // P1OUT &= ~BIT4;
    // P1OUT &= ~BIT5;
}

void epd_reset(void)
{
    // power up
    EPD_PWR_PORT &= ~EPD_PWR_PIN;  

   // pre reset init
    EPD_RST_PORT |= EPD_RST_PIN;
    EPD_DCn_PORT |= EPD_DCn_PIN;
    EPD_DIN_PORT |= EPD_DIN_PIN;
    EPD_CLK_PORT |= EPD_CLK_PIN;
    EPD_CSN_PORT |= EPD_CSN_PIN;

    // 4 wire bus
    EPD_BS1_PORT &= ~EPD_BS1_PIN;  
    delay_ms(5);
    // reset
    EPD_RST_PORT |= EPD_RST_PIN;
    delay_ms(200);
    EPD_RST_PORT &= ~EPD_RST_PIN;  
    delay_ms(10);
    EPD_RST_PORT |= EPD_RST_PIN;
    delay_ms(200);

}

void epd_init(void)
{
#if 1
// lut from OTP
// https://github.com/waveshareteam/Pico_ePaper_Code/blob/main/c/lib/e-Paper/EPD_2in13d.c

    epd_send_cmd(0x01);  // power settings
    epd_send_data(0x03);
    epd_send_data(0x00);
    epd_send_data(0x2b);
    epd_send_data(0x2b);
    epd_send_data(0x03);

    epd_send_cmd(0x06); // booster soft start
    epd_send_data(0x17);
    epd_send_data(0x17);
    epd_send_data(0x17);

    epd_send_cmd(0x04);  // power on
    epd_wait_busy();

    epd_send_cmd(0x00);  // panel setting
    epd_send_data(0xbf);  //LUT from OTP，128x296
    epd_send_data(0x0e);  //VCOM to 0V fast

    epd_send_cmd(0x30);  // PLL control
    epd_send_data(0x3a); // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ

    epd_send_cmd(0x61);   // resolution setting
    epd_send_data(EPD_WIDTH);
    epd_send_data((EPD_HEIGHT >> 8) & 0xff);
    epd_send_data(EPD_HEIGHT & 0xff);  

    epd_send_cmd(0x82);   // VCM_DC setting
    epd_send_data(0x28);


#endif     

// lut from OTP
#if 0
    epd_send_cmd(0x06); // booster soft start
    epd_send_data(0x17);
    epd_send_data(0x17);
    epd_send_data(0x17);

    epd_send_cmd(0x04);  // power on
    delay_ms(100);
    epd_wait_busy();

    epd_send_cmd(0x00);  // panel setting
    epd_send_data(0x0f);

    epd_send_cmd(0x61);   // resolution setting
    epd_send_data(0x60);
    epd_send_data(0x00);
    epd_send_data(0xcb);  

    epd_send_cmd(0x50);   // VCOM 
    epd_send_data(0xd7);  
#endif
}

void epd_sleep(void) 
{
    epd_send_cmd(0x50);
    epd_send_data(0xf7);
    epd_send_cmd(0X02);//power off
    epd_wait_busy();//waiting for the electronic paper IC to release the idle signal
    epd_send_cmd(0X07);//deep sleep
    epd_send_data(0xA5);
}

void epd_spi_write(const uint8_t data)
{
    uint8_t bit;

    for (bit = 0x80; bit > 0; bit >>= 1) {
        if (data & bit) {
            EPD_DIN_PORT |= EPD_DIN_PIN;  // Set MOSI high
            //P1OUT |= BIT4;
        } else {
            EPD_DIN_PORT &= ~EPD_DIN_PIN; // Set MOSI low
            //P1OUT &= ~BIT4;
        }
        __delay_cycles(5);
        EPD_CLK_PORT |= EPD_CLK_PIN;     // Set Clock high
        //P1OUT|= BIT5;
        __delay_cycles(5);
        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
        //P1OUT&=~BIT5;
        __delay_cycles(5);
    }
}

void epd_write(const uint8_t val)
{
    EPD_CSN_PORT &= ~EPD_CSN_PIN;
    __delay_cycles(1);
    epd_spi_write(val);
    __delay_cycles(1);
    EPD_CSN_PORT |= EPD_CSN_PIN;
}

void epd_send_cmd(const uint8_t cmd)
{
    EPD_DCn_PORT &= ~EPD_DCn_PIN; // command
    __delay_cycles(1);
    epd_write(cmd);
}

void epd_send_data(const uint8_t data)
{
    EPD_DCn_PORT |= EPD_DCn_PIN;  // data
    __delay_cycles(1);
    epd_write(data);
}

void epd_wait_busy(void)
{
    epd_send_cmd(0x71);
    while( (EPD_BUS_PORT & EPD_BUS_PIN) != 0){
        epd_send_cmd(0x71);
        uart_putc('.');
        delay_ms(100);
        }
    delay_ms(200);   
}

void epd_clear_disp(void)
{
    uart_putstring("EPD writing data...\r\n");

    epd_send_cmd(0x10);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0x0F);  
    }  
    uart_putstring("old data done.\r\n");
    delay_ms(2);
    epd_send_cmd(0x13);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0xF0);  
    }
    uart_putstring("new data done.\r\n");
    uart_putstring("Refreshing...\r\n");
    epd_send_cmd(0X50);			//VCOM AND DATA INTERVAL SETTING
    epd_send_data(0xb7);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    epd_send_cmd(0x12);
    delay_ms(100);
    epd_wait_busy();

}

// // Timer A0 interrupt service routine

// __attribute__ ( ( interrupt( TIMER0_A0_VECTOR ) ) )
// void TIMER1_A0_ISR( void )
// {
//     P1OUT ^= BIT0; // Toggle P1.0 (LED)
//     //toggle_led('b');
// }
