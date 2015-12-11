#include "port.h"
#include "pin_mapping.h"
#include "ad84xx.h"

#define DAC_VADJ_SPI_BITRATE    10000000
#define DAC_VADJ_FRAME_SIZE   10

void dac_vadj_init( void )
{
    gpio_set_pin_dir( GPIO_DAC_VADJ_RST_PORT, GPIO_DAC_VADJ_RST_PIN, OUTPUT);

    /* Reset the AD84xx while initializing the SSP interface */
    gpio_set_pin_state( GPIO_DAC_VADJ_RST_PORT, GPIO_DAC_VADJ_RST_PIN, LOW);

    spi_config( DAC_VADJ_SPI_BITRATE, DAC_VADJ_FRAME_SIZE, SPI_MASTER, SPI_POLLING );

    gpio_set_pin_state( GPIO_DAC_VADJ_RST_PORT, GPIO_DAC_VADJ_RST_PIN, HIGH);
}

void dac_vadj_config( uint8_t addr, uint8_t val )
{
    uint8_t data[2] = { (val), (addr & 0x3) };
    spi_assertSSEL();
    spi_write( &data[0], sizeof(data) );
    spi_deassertSSEL();
}
