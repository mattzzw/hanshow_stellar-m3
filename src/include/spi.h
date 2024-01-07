#include <stdint.h>
#include "msp430.h"

#define SPI_PORT     P3OUT
#define SPI_CLK_PIN  BIT0
#define SPI_CSN_PIN  BIT3
#define SPI_SI_PIN   BIT4
#define SPI_SO_PIN   BIT5

void setup_spi(void);
uint8_t spi_raw_transfer_byte(uint8_t);
void spi_transfer_bytes(uint8_t num_bytes, uint8_t * rx_data, uint8_t * tx_data);
uint8_t spi_release_pwrdn_get_dev_id(void);
void spi_read( uint32_t addr, uint8_t *rbuf, uint16_t len);
void spi_write( uint32_t addr, uint8_t *wbuf, uint16_t len);
void spi_wait_wip_clear(void);
void spi_sector_erase(uint32_t addr);


