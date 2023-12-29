#include <msp430.h>
#include <stdint.h>
#include "spi.h"


uint8_t rd[10], wr[10];


void setup_spi(void)
{
/*
The recommended USCI initialization/re-configuration process is:
1. Set UCSWRST (BIS.B #UCSWRST,&UCxCTL1)
2. Initialize all USCI registers with UCSWRST=1 (including UCxCTL1)
3. Configure ports
4. Clear UCSWRST via software (BIC.B #UCSWRST,&UCxCTL1)
5. Enable interrupts (optional) via UCxRXIE and/or UCxTXIE
*/
    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 =  UCCKPL + UCMSB + UCMST + UCMODE_0 + UCSYNC;  // MSB first, master mode, 4-pin with active low enable, synchr mode

    P3DIR |= SPI_CSN_PIN;                               // driving CSn by hand
    P3SEL |= SPI_SI_PIN + SPI_SO_PIN+ SPI_CLK_PIN;      // primary peripheral
    P3SEL2 &= ~(SPI_SI_PIN + SPI_SO_PIN+ SPI_CLK_PIN);

    UCA0CTL1 = UCSSEL_3;

}


uint8_t  spi_raw_transfer_byte(uint8_t b)
{
    UCA0TXBUF = b;
    while(!(UCA0STAT & UCBUSY)){};
    return UCA0RXBUF;
}

void spi_transfer_bytes(uint8_t num_bytes, uint8_t * rx_data, uint8_t * tx_data)
{
int i;
    P3OUT &= ~SPI_CSN_PIN;
    for(i = 0; i < num_bytes; i++ ){
        rx_data[i] = spi_raw_transfer_byte(tx_data[i]);
    }
    P3OUT |= SPI_CSN_PIN;

}

uint8_t spi_release_pwrdn_get_dev_id(void){
    wr[0]=0xAB;
    wr[1]= 0;
    wr[2]= 0;
    wr[3]= 0;
    wr[4]= 0;
    spi_transfer_bytes(5, rd, wr);
    return rd[4];
}

void spi_read( uint32_t addr, uint8_t *rbuf, uint16_t len){
    int16_t i;
    P3OUT &= ~SPI_CSN_PIN;
    spi_raw_transfer_byte(0x03); // read cmd
    spi_raw_transfer_byte(addr >> 16);
    spi_raw_transfer_byte(addr >> 8);
    spi_raw_transfer_byte(addr & 0xff);
    for( i = 0; i < len; i++){
        rbuf[i] = spi_raw_transfer_byte(0);
    }
    P3OUT |= SPI_CSN_PIN;

}