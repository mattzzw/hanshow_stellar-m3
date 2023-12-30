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
}

void epd_reset(void)
{
    EPD_BS1_PORT &= ~EPD_BS1_PIN;  // 4 wire bus
    EPD_PWR_PORT &= ~EPD_PWR_PIN;  // switch on pwr 
    EPD_RST_PORT |= EPD_RST_PIN;
    delay_ms(200);
    EPD_RST_PORT &= ~EPD_RST_PIN;  // reset 
    delay_ms(10);
    EPD_RST_PORT |= EPD_RST_PIN;
    delay_ms(200);

}

void epd_init(void)
{
    epd_send_cmd(0x06); // booster soft start
    epd_send_data(0x17);
    epd_send_data(0x17);
    epd_send_data(0x17);

    epd_wait_busy();
    epd_send_cmd(0x04);  // power on
    epd_wait_busy();

    epd_send_cmd(0x00);  // panel setting
    epd_send_data(0x0f);

    epd_send_cmd(0x61);   // resolution setting
    epd_send_data(0x60);
    epd_send_data(0x00);
    epd_send_data(0xcb);  

    epd_send_cmd(0x50);   // VCOM 
    epd_send_data(0xd7);  

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
            B_LED_PORT |= B_LED_PIN;
        } else {
            EPD_DIN_PORT &= ~EPD_DIN_PIN; // Set MOSI low
            B_LED_PORT &= ~B_LED_PIN;
        }

        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
        G_LED_PORT &= ~G_LED_PIN;
        __delay_cycles(1);
        EPD_CLK_PORT |= EPD_CLK_PIN;     // Set Clock high
        G_LED_PORT |= G_LED_PIN;
        __delay_cycles(1);
    }

}

void epd_write(const uint8_t val)
{
    EPD_CSN_PORT &= ~EPD_CSN_PIN;
    epd_spi_write(val);
    EPD_CSN_PORT |= EPD_CSN_PIN;
}

void epd_send_cmd(const uint8_t cmd)
{
    EPD_DCn_PORT &= ~EPD_DCn_PIN; // command
    epd_write(cmd);
}

void epd_send_data(const uint8_t data)
{
    EPD_DCn_PORT |= EPD_DCn_PIN;  // data
    epd_write(data);
}

void epd_wait_busy(void)
{
        
    uart_putstring("EPD busy...\r\n");
    while( (EPD_BUS_PORT & EPD_BUS_PIN) != 0){
        }
    uart_putstring("EPD not busy.\r\n");
       
}

void epd_clear_disp(void)
{
    uart_putstring("EPD writing data...\r\n");

    epd_send_cmd(0x10);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0xFF);  
    }  
    uart_putstring("Done.\r\n");
    delay_ms(2);
    // epd_send_cmd(0x13);
    // for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
    //     epd_send_data(0xFF);  
    // }
    epd_send_cmd(0x12);
    delay_ms(100);
    epd_wait_busy();
}

