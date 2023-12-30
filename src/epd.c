#include <stdint.h>
#include "msp430.h"
#include "epd.h"
#include "led.h"
#include "tools.h"

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
    epd_send_data(0x89);

    epd_send_cmd(0x61);   // resolution setting
    epd_send_data(0x68);
    epd_send_data(0x00);
    epd_send_data(0xd4);  

    epd_send_cmd(0x50);   // VCOM 
    epd_send_data(0x87);  

    epd_send_cmd(0x12); // refresh
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
            G_LED_PORT |= G_LED_PIN;
        } else {
            EPD_DIN_PORT &= ~EPD_DIN_PIN; // Set MOSI low
            G_LED_PORT &= !G_LED_PIN;
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

void epd_clear_disp(void)
{
    epd_send_cmd(0x10);           
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0xFF);  
    }  
    delay_ms(2);
    epd_send_cmd(0x13);
    for(int i = 0; i < EPD_WIDTH * EPD_HEIGHT / 8; i++) {
        epd_send_data(0xFF);  
    }
    epd_send_cmd(0x12);
    delay_ms(100);
    epd_wait_busy();
}