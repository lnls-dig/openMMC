/*
 *   openMMC -- Open Source modular IPM Controller firmware
 *
 *   Copyright (C) 2015  Henrique Silva <henrique.silva@lnls.br>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/* Project Includes */
#include "port.h"
#include "ad84xx.h"

#define DAC_AD84XX_SPI_BITRATE    10000000
#define DAC_AD84XX_FRAME_SIZE     10

void dac_ad84xx_init( void )
{
    /* Reset the AD84xx while initializing the SSP interface */
    gpio_set_pin_state( PIN_PORT(GPIO_DAC_VADJ_RST), PIN_NUMBER(GPIO_DAC_VADJ_RST), GPIO_LEVEL_LOW);

    spi_config( DAC_AD84XX_SPI_BITRATE, DAC_AD84XX_FRAME_SIZE, SPI_MASTER, SPI_POLLING );

    gpio_set_pin_state( PIN_PORT(GPIO_DAC_VADJ_RST), PIN_NUMBER(GPIO_DAC_VADJ_RST), GPIO_LEVEL_HIGH);
}

void dac_ad84xx_set_val( uint8_t addr, uint8_t val )
{
    uint16_t data = (addr << 8) | (val);
    spi_assertSSEL();
    spi_write( (uint8_t *) &data, sizeof(data) );
    spi_deassertSSEL();
}

void dac_ad84xx_set_res( uint8_t addr, uint16_t res )
{
    uint16_t data = 0;

    if (res > 50) {
        data = ((res - 50)*256)/1000;
    }

    /* DAC value is limited to 256 steps */
    data &= 0xFF;

    dac_ad84xx_set_val( addr, data );
}
