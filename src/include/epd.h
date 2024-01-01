#include <stdint.h>
#include "msp430.h"

#define EPD_HEIGHT      212
#define EPD_WIDTH       104

#define EPD_PWR_PORT    P1OUT
#define EPD_DIN_PORT    P2OUT
#define EPD_CLK_PORT    P2OUT
#define EPD_CSN_PORT    P3OUT
#define EPD_DCn_PORT    P3OUT
#define EPD_RST_PORT    P4OUT
#define EPD_BUS_PORT    P4IN  
#define EPD_BS1_PORT    P4OUT

#define EPD_PWR_PIN BIT0
#define EPD_DIN_PIN BIT4
#define EPD_CLK_PIN BIT3
#define EPD_CSN_PIN BIT7
#define EPD_DCn_PIN BIT6
#define EPD_RST_PIN BIT7
#define EPD_BUS_PIN BIT6
#define EPD_BS1_PIN BIT5

void epd_setup_pins(void);
void epd_reset(void);
void epd_init(void);
void epd_send_cmd(const uint8_t);
void epd_send_data(const uint8_t);
void epd_write(const uint8_t);
void epd_spi_write(const uint8_t);
void epd_wait_busy(void);
void epd_clear_disp(void);
void epd_sleep(void);
