#include <stdint.h>
#include <stdio.h>  // FIXME Debug
#include "msp430.h"
#include "epd.h"
#include "led.h"    // FIXME Debug
#include "tools.h"
#include "uart.h"   // FIXME Debug

void epd_setup_pins(void)
{
    // configure pin directions
    P1DIR |= EPD_PWR_PIN;
    P2DIR |= EPD_DIN_PIN;
    P2DIR |= EPD_CLK_PIN;
    P3DIR |= EPD_CSN_PIN;
    P3DIR |= EPD_DCn_PIN;
    P4DIR |= EPD_RST_PIN;

    P4DIR &= ~EPD_BUS_PIN; // input
//    P4REN |= EPD_BUS_PIN;  // enable pullup/down
//    P4OUT |= EPD_BUS_PIN;  // pull-up
    
    P4DIR |= EPD_BS1_PIN;
    P4DIR |= EPD_HLT_PIN;

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

   // pre reset init
    EPD_RST_PORT |= EPD_RST_PIN;
    EPD_DCn_PORT |= EPD_DCn_PIN;
    EPD_DIN_PORT |= EPD_DIN_PIN;
    EPD_CLK_PORT |= EPD_CLK_PIN;
    EPD_CSN_PORT |= EPD_CSN_PIN;
    EPD_HLT_PORT |= EPD_HLT_PIN;

    // power up
    EPD_PWR_PORT &= ~EPD_PWR_PIN;  
    EPD_HLT_PORT &= ~EPD_HLT_PIN;  

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

int i;
uint8_t str[20];

// SSD1675B init

    epd_send_cmd(0x12); // sw reset
    delay_ms(10);

    epd_send_cmd(0x2f);
    i=epd_read();
    sprintf(str, "FLG: 0x%02x\n\r", i);
    uart_putstring(str);

/*
    epd_send_cmd(0x01); // gate driver output
    epd_send_data(0x27);
    epd_send_data(0x01);
    epd_send_data(0x00);
*/
    epd_send_cmd(0x11); // data entry
    epd_send_data(0x03);

    epd_send_cmd(0x3c); // set border
    epd_send_data(0x05);

    epd_send_cmd(0x2c); // write vcom
    epd_send_data(0x36);
    epd_send_cmd(0x03); // gate voltage
    epd_send_cmd(0x17);

    epd_send_cmd(0x04); // source voltage
    epd_send_data(0x41);
    epd_send_data(0x00);
    epd_send_data(0x32);

    // write image data
    epd_send_cmd(0x4e);  // RAM addr. x
    epd_send_data(0x01);
    epd_send_cmd(0x4f);  // RAM addr. y
    epd_send_data(0x00);
    epd_send_data(0x00);


    epd_send_cmd(0x22); // drive display
    epd_send_data(0xf4);

    epd_send_cmd(0x2f);

    i=epd_read();
    sprintf(str, "FLG: 0x%02x\n\r", i);
    uart_putstring(str);

    epd_send_cmd(0x20);
    delay_ms(1);
    epd_wait_busy;

    epd_send_cmd(0x2f);
    i=epd_read();
    sprintf(str, "FLG: 0x%02x\n\r", i);
    uart_putstring(str);


    epd_send_cmd(0x10);  // deep sleep
    epd_send_data(0x01);
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

        EPD_CLK_PORT |= EPD_CLK_PIN;     // Set Clock high
        //P1OUT|= BIT5;
        __delay_cycles(5);
        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
        //P1OUT&=~BIT5;
        __delay_cycles(5);

    }
}

uint8_t epd_spi_read()
{
    uint8_t data = 0;
    uint8_t bit;

    // switch pin to input
    P2DIR &= ~EPD_DIN_PIN;

    for(bit = 0x80; bit > 0; bit >>= 1){
        EPD_CLK_PORT |= EPD_CLK_PIN;     // Set Clock high
        //P1OUT|= BIT5;
        __delay_cycles(5);

        if (P2IN & EPD_DIN_PIN){
            data |= bit; 
        }
        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
        //P1OUT&=~BIT5;
        __delay_cycles(5);
    }
    // switch back to output
    P2DIR |= EPD_DIN_PIN;

    return data;
}

void epd_write(const uint8_t val)
{
    EPD_CSN_PORT &= ~EPD_CSN_PIN;
    __delay_cycles(1);
    epd_spi_write(val);
    __delay_cycles(1);
    EPD_CSN_PORT |= EPD_CSN_PIN;
}

uint8_t epd_read(void)
{
    uint8_t data;

    EPD_DCn_PORT |= EPD_DCn_PIN;
    EPD_CSN_PORT &= ~EPD_CSN_PIN;
    __delay_cycles(1);
    data = epd_spi_read();
    __delay_cycles(1);
    EPD_CSN_PORT |= EPD_CSN_PIN;
    return data;
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
    while( (EPD_BUS_PORT & EPD_BUS_PIN) == 1){
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
 //   epd_send_cmd(0X50);			//VCOM AND DATA INTERVAL SETTING
 //   epd_send_data(0xb7);		//WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    epd_send_cmd(0x12);
    delay_ms(100);
    epd_wait_busy();

}
