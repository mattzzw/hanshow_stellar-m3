#include <stdint.h>
#include "msp430.h"
#include "epd.h"
#include "led.h"

void epd_setup_pins(void)
{
    //PxSEL are 0 after reset
    
    P1DIR |= EPD_PWR_PIN;
    P2DIR |= EPD_DIN_PIN;
    P2DIR |= EPD_CLK_PIN;
    P3DIR |= EPD_CSN_PIN;
    P3DIR |= EPD_DCn_PIN;
    P4DIR |= EPD_RST_PIN;
    P4DIR &= ~EPD_BUS_PIN; // input
    P4DIR |= EPD_BS1_PIN;
}

void epd_reset(void)
{
    EPD_BS1_PORT &= ~EPD_BS1_PIN;  // 4 wire bus
    EPD_PWR_PORT &= ~EPD_PWR_PIN;  // switch on pwr
    EPD_RST_PORT &= ~EPD_RST_PIN;  // reset 
    __delay_cycles(0x32);
    EPD_RST_PORT |= EPD_RST_PIN;
    __delay_cycles(0x32);

}

void epd_init(void)
{
    epd_wait_busy();
    epd_send_cmd(0x12);  // soft reset
    epd_wait_busy();

    epd_send_cmd(0x06);  // driver output ctrl
    epd_send_data(0x17);
    epd_send_data(0x17);
    epd_send_data(0x17);

    epd_send_cmd(0x11);   // data entry mode
    epd_send_data(0x01);

    epd_send_cmd(0x44);   // RAM-x addr start pos
    epd_send_data(0x00);
    epd_send_data(0x0f);  //0x0C-->(15+1)*8=128

    epd_send_cmd(0x45);   // RAM-y addr start pos
    epd_send_data(0xf9);  //0xF9-->(249+1)=250
    epd_send_data(0x00);
    epd_send_data(0x00);

    epd_send_cmd(0x3c);   // border waveform 
    epd_send_data(0xc0);

    epd_send_cmd(0x0c);  // soft start
    epd_send_data(0x8b);
    epd_send_data(0x9c);
    epd_send_data(0x96);
    epd_send_data(0x0f);

    epd_send_cmd(0x20);   // master update




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

        EPD_CLK_PORT &= ~EPD_CLK_PIN;      // Set Clock low
        __delay_cycles(2);
        EPD_CLK_PORT |= EPD_CLK_PIN;     // Set Clock high
        __delay_cycles(2);
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
    EPD_DCn_PORT |= EPD_DCn_PIN;
    epd_write(data);
}

void epd_wait_busy(void)
{
    while( (EPD_BUS_PORT & EPD_BUS_PIN) != 0){
        toggle_led('g');
    }
       
}