#include <stdint.h>
#include "msp430.h"
#include "epd.h"
#include "led.h"    // FIXME Debug
#include "tools.h"



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
    delay_ms(10);
    // reset
    EPD_RST_PORT |= EPD_RST_PIN;
    delay_ms(200);
    EPD_RST_PORT &= ~EPD_RST_PIN;  
    delay_ms(20);
    EPD_RST_PORT |= EPD_RST_PIN;
    delay_ms(200);

}

void epd_init(void)
{

// SSD1680 init

    delay_ms(10);
    epd_send_cmd(0x12); // sw reset
    delay_ms(10);

    epd_send_cmd(0x74);  // set analog ctrl
    epd_send_data(0x54);

    epd_send_cmd(0x7e);  // set digital ctrl
    epd_send_data(0x3b);

    epd_send_cmd(0x2b);  // VCOM ctrl
    epd_send_data(0x04);
    epd_send_data(0x63);

    epd_send_cmd(0x0c);  // booster soft start ctrl.
    epd_send_data(0x8b);
    epd_send_data(0x9c);
    epd_send_data(0x96);
    epd_send_data(0x0f);


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

    epd_send_cmd(0x18);  // internal temp sensor
    epd_send_data(0x80);

    // write image data
    epd_send_cmd(0x4e);  // RAM addr. x
    epd_send_data(0x00);

    epd_send_cmd(0x4f);  // RAM addr. y
    epd_send_data(0x00);
    epd_send_data(0x00);
    epd_send_data(0xfe);

    epd_send_cmd(0x44);  // set ram x start/stop pos
    epd_send_data(0x00);
    epd_send_data((EPD_WIDTH -1)>>3); 
    
    epd_send_cmd(0x45);  // set ram y start/stop pos
    epd_send_data(0x00);
    epd_send_data(0x00);
    epd_send_data((EPD_HEIGHT-1) & 0xff);
    epd_send_data(EPD_HEIGHT>>8);

    epd_send_cmd(0x01); // gate driver output
    epd_send_data((EPD_HEIGHT + 1) & 0xff);
    epd_send_data((EPD_HEIGHT + 1) >> 8);
    epd_send_data(0x00);

}



void epd_update_display(void)
{
    epd_send_cmd(0x22); 
    epd_send_data(0xc7);  // 0xc7 or 0xf7? 

    epd_send_cmd(0x20);
    delay_ms(1);
    epd_wait_busy();

}

void epd_sleep(void) 
{
    epd_send_cmd(0x10);  // deep sleep
    epd_send_data(0x01);
}


void epd_spi_write(const uint8_t data)
{
    uint8_t bit;

    for (bit = 0x80; bit > 0; bit >>= 1) {
        if (data & bit) {
            EPD_DIN_PORT |= EPD_DIN_PIN;  // Set MOSI high
        } else {
            EPD_DIN_PORT &= ~EPD_DIN_PIN; // Set MOSI low
        }

        EPD_CLK_PORT |= EPD_CLK_PIN;     // Set Clock high
       // __delay_cycles(1);
        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
       // __delay_cycles(1);

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
       // __delay_cycles(1);

        if (P2IN & EPD_DIN_PIN){
            data |= bit; 
        }
        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
       // __delay_cycles(1);
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
    while( EPD_BUS_PORT & EPD_BUS_PIN ){
        delay_ms(100);
        }
    delay_ms(200);   
}

void epd_clear_disp(void)
{

        // write image data
    epd_send_cmd(0x4e);  // RAM addr. x
    epd_send_data(0x00);

    epd_send_cmd(0x4f);  // RAM addr. y
    epd_send_data(0x00);
    epd_send_data(0x00);
    epd_send_data(0xfe);

    epd_send_cmd(0x24);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0xff);  
    }  
    epd_send_cmd(0x26);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0xff);  
    }  
    delay_ms(2);
}


void epd_write_buf(volatile uint8_t *buf){

        // write image data
    epd_send_cmd(0x4e);  // RAM addr. x
    epd_send_data(0x00);

    epd_send_cmd(0x4f);  // RAM addr. y
    epd_send_data(0x00);
    epd_send_data(0x00);
    epd_send_data(0xfe);

    epd_send_cmd(0x24);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(buf[i]);  
    }  
    epd_send_cmd(0x26);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(buf[i]);  
    }  
    delay_ms(2);
}